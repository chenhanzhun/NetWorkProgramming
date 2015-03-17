#ifndef PROCESS_H
#define PROCESS_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <assert.h>
#include <sys/epoll.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/wait.h>

class process
{
    public:
        pid_t m_pid;
        int m_pipefd[2];

    public:
        process():m_pid(-1){}
};

template<typename T>
class processpool
{
    private:
        /* 进程池允许的最大子进程的数量 */
        static const int MAX_PROCESS_NUMBER = 16;
        /* 每个子进程最多处理的客户数量 */
        static const int USER_PER_PROCESS = 65536;
        /* epoll 最多能处理的事件数量 */
        static const int MAX_EVENT_NUMBER = 10000;

        /* 进程池中的进程总数 */
        int m_process_number;
        /* 子进程在进程池中的序号，从0开始 */
        int m_index;
        /* 每个进程都有一个epoll内核事件表 */
        int m_epollfd;
        /* 监听套接字 */
        int m_listenfd;
        /* 控制子进程停止或运行 */
        int m_stop;
        /* 保存所有子进程的描述消息 */
        process *m_sub_process;
        /* 进程池静态实例 */
        static processpool<T> *m_instance;

    private:
        /* 构造函数定义为私有，必须调用后面的create静态函数创建进程池实例 */
        processpool(int listenfd, int process_number = 8);

    public:
        /* 单体实例，以保证程序最多创建一个processpool实例，这是正确处理信号的必要条件 */
        static processpool<T> *create(int listenfd, int process_number = 8)
        {
            if(!m_instance)
                m_instance = new processpool<T>(listenfd, process_number);
            return m_instance;
        }

        ~processpool()
        {
            delete[]m_sub_process;
        }

        /* 启动进程池 */
        void run();

    private:
        void setup_sig_pipe();/* 设置信号管道 */
        void run_parent();/* 运行父进程 */
        void run_child();/* 运行子进程 */
};

template<typename T>
processpool<T>* processpool<T>::m_instance = NULL;
/* 信号管道 */
static int sig_pipefd[2];

/* 设置描述符为非阻塞模式，并返回原始IO模式值 */
static int setnonblocking(int fd)
{
    int old_option, new_option;
    /* 获取描述符状态标志 */
    if( (old_option = fcntl(fd, F_GETFL, 0)) < 0)
    {
        perror("fcntl error");
        exit(1);
    }
    /* 添加非阻塞状态标志 */
    new_option = old_option | O_NONBLOCK;
    /* 设置描述符状态标志 */
    if(fcntl(fd, F_SETFL, new_option) < 0)
    {
        perror("fcntl set O_NONBLOCK error");
        exit(1);
    }

    return old_option;
}

/* 将fd描述符注册到epoll内核事件表 */
static void addfd(int epollfd, int fd)
{
    struct epoll_event event;
    event.data.fd = fd;/* 设置待处理的描述符 */
    event.events = EPOLLIN | EPOLLET;/* 可读事件发生，并设置其为ET工作方式（高速工作方式），默认是LT */
    if(epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event) < 0)/* 注册epoll事件 */
    {
        perror("epoll ADD error");
        exit(1);
    }
    setnonblocking(fd);
}

/* 从epoll内核事件表删除描述符fd所注册的事件 */
static void remove(int epollfd, int fd)
{
    if(epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, 0) <0)
    {
        perror("epoll remove error");
        exit(1);
    }
    close(fd);
}

/* 信号处理函数 */
static void sig_handler(int signo)
{
    int save_errno = errno;
    int msg = signo;
    /* 将信号写入到信号管道 */
    send(sig_pipefd[1], (char *)&msg, 1, 0);
    errno = save_errno;
}

/* 捕获信号，并处理该信号 */
static void addsig(int signo, void(handler)(int), bool restart = true)
{
    struct sigaction sa;
    memset(&sa, '\0', sizeof(sa));
    sa.sa_handler = handler;/* 指定信号处理函数 */
    if(restart)/* 若设置可重启，则标志为可重启 */
        sa.sa_flags |= SA_RESTART;
    sigfillset(&sa.sa_mask);/* 初始化信号集，使其包括所有信号 */
    assert( sigaction(signo, &sa, NULL) != -1);
}

/* 构造进程池，监听套接字listenfd必须在这之前创建
 * process_number指定进程池中子进程的数量 */
template<typename T>
processpool<T>::processpool(int listenfd, int process_number)
    :m_listenfd(listenfd), m_process_number(process_number), m_index(-1),
    m_stop(false)
{
    assert( (process_number > 0) && (process_number <= MAX_PROCESS_NUMBER));

    m_sub_process = new process[process_number];
    assert(m_sub_process);

    /* 创建子进程，并建立子进程与父进程之间的管道 */
    for(int i = 0; i < process_number; ++i)
    {
        /* 创建一对无名的套接字描述符,只能在UNIX域创建
         * 这里的功能是创建一个全双工通信的管道
         * 即父进程与每个子进程都有一个对应的管道 */
        int ret = socketpair(PF_UNIX, SOCK_STREAM, 0, m_sub_process[i].m_pipefd);
        assert(ret == 0);

        /* 创建子进程 */
        m_sub_process[i].m_pid = fork();
        assert(m_sub_process[i].m_pid >= 0);

        if(m_sub_process[i].m_pid > 0)
        {/* 在父进程中关闭管道的写入端 */
            close(m_sub_process[i].m_pipefd[1]);
            continue;
        }
        else
        {/* 子进程中关闭管道的读取端，并标志子进程在进程池中的序号 */
            close(m_sub_process[i].m_pipefd[0]);
            m_index = i;
            break;
        }
    }
}

/* 统一事件源 */
template<typename T>
void processpool<T>::setup_sig_pipe()
{
    /* 创建epoll事件监听表和信号管道 */
    m_epollfd = epoll_create(5);
    assert(m_epollfd != -1);

    /* 创建全双工的信号管道 */
    int ret = socketpair(PF_UNIX, SOCK_STREAM, 0, sig_pipefd);
    assert(ret != -1);

    /* 设置信号管道的写入端为非阻塞模式 */
    setnonblocking(sig_pipefd[1]);
    /* 把信号管道的读取端描述符注册到epoll事件表中 */
    addfd(m_epollfd, sig_pipefd[0]);

    /* 捕获信号，并设置信号处理函数 */
    addsig(SIGCHLD, sig_handler);/* 该信号是进程终止时，将SGICHLD信号发送给其父进程 */
    addsig(SIGINT, sig_handler);/* 键入中断时产生该信号，将该信号发送给前台进程组的每一个进程 */
    addsig(SIGTERM, sig_handler);/* 由kill命令发送的信号 */
    addsig(SIGPIPE, sig_handler);/* 写到管道时，读进程已终止，则产生该信号 */
}

/* 父进程中m_index值为-1，子进程中m_index大于等于0，根据这个值可以决定运行子进程还是父进程 */
template<typename T>
void processpool<T>::run()
{
    if(m_index != -1)
    {
        run_child();
        return;
    }
    run_parent();
}

/* 子进程的运行程序 */
template<typename T>
void processpool<T>::run_child()
{
    setup_sig_pipe();

    /* 每个子进程都通过其在进程池中的序号值m_index找到与父进程通信的管道 */
    int pipefd = m_sub_process[m_index].m_pipefd[1];

    /* 子进程描述符注册到epoll事件表 */
    addfd(m_epollfd, pipefd);

    /* 每个epoll最多处理的事件数 */
    struct epoll_event events[MAX_EVENT_NUMBER];
    /* 每个子进程最多处理的客户数量 */
    T* users = new T[USER_PER_PROCESS];
    assert(users);

    int number = 0;
    int ret = -1;

    while(!m_stop)
    {
        /* 轮询IO事件的发生，并返回发生事件数 */
        number = epoll_wait(m_epollfd, events, MAX_EVENT_NUMBER, -1);
        if( (number < 0) && (errno != EINTR))
        {
            printf("epoll_wait failure\n");
            break;
        }

        for(int i = 0; i < number; i++)
        {
            int sockfd = events[i].data.fd;
            /* pipefd管道可读 */
            if( (sockfd == pipefd) && (events[i].events & EPOLLIN))
            {
                int client = 0;
                /* 从父、子进程之间的管道读取数据，若读取成功，表示有新客户连接请求到达 */
                ret = recv(sockfd, (char *)&client, sizeof(client), 0);
                if(((ret < 0) &&(errno != EAGAIN)) || ret == 0)
                    continue;
                else
                {
                    struct sockaddr_in cliaddr;
                    socklen_t clilen = sizeof(cliaddr);

                    /* 接受新客户的连接请求 */
                    int connfd = accept(m_listenfd, (struct sockaddr*)&cliaddr, &clilen);
                    if(connfd < 0)
                    {
                        printf("accept error, errno is: %d\n", errno);
                        continue;
                    }
                    addfd(m_epollfd, connfd);
                    users[connfd].init(m_epollfd, connfd, cliaddr);
                }
            }
            /* 处理进程所接收的消息 */
            else if( (sockfd == sig_pipefd[0]) && (events[i].events & EPOLLIN))
            {
                char signals[1024];
                ret = recv(sig_pipefd[0], signals, sizeof(signals), 0);
                if(ret < 0)
                    continue;
                else
                {
                    for(int i = 0; i < ret; i++)
                    {
                        switch(signals[i])
                        {
                            case SIGCHLD:
                                {
                                    pid_t pid;
                                    int stat;
                                    while( (pid = waitpid(-1, &stat, WNOHANG)) > 0)
                                        continue;
                                    break;
                                }
                            case SIGTERM:
                            case SIGINT:
                                {
                                    m_stop = true;
                                    break;
                                }
                            default:
                                break;
                        }
                    }
                }
            }
            else if(events[i].events & EPOLLIN)
                users[sockfd].process();
            else
                continue;
        }
    }
    delete[]users;
    users = NULL;
    close(pipefd);
    close(m_epollfd);
}

/* 父进程运行程序 */
template<typename T>
void processpool<T>::run_parent()
{
    setup_sig_pipe();

    addfd(m_epollfd, m_listenfd);

    struct epoll_event events[MAX_EVENT_NUMBER];
    int sub_process_counter = 0;
    int new_conn = 1;
    int number = 0;
    int ret = -1;

    while(!m_stop)
    {
        number = epoll_wait(m_epollfd, events, MAX_EVENT_NUMBER, -1);
        if( (number < 0) && (errno != EINTR))
        {
            printf("epoll_wait error\n");
            break;
        }

        for(int i = 0; i < number; i++)
        {
            int sockfd = events[i].data.fd;
            if(sockfd == m_listenfd)
            {
                int i = sub_process_counter;
                do
                {
                    if(m_sub_process[i].m_pid != -1)
                        break;
                    i = (i+1)%m_process_number;
                }while(i != sub_process_counter);

                if(m_sub_process[i].m_pid == -1)
                {
                    m_stop = true;
                    break;
                }
                sub_process_counter = (i+1)%m_process_number;

                send(m_sub_process[i].m_pipefd[0], (char *)&new_conn, sizeof(new_conn), 0);
                printf("send request to child %d\n", i);
            }
            else if( (sockfd == sig_pipefd[0]) && (events[i].events & EPOLLIN))
            {
                char signals[1024];
                ret = recv(sig_pipefd[0], signals, sizeof(signals), 0);
                if(ret <= 0)
                    continue;
                else
                {
                    for(int i = 0; i < ret; i++)
                    {
                        switch(signals[i])
                        {
                            case SIGCHLD:
                                {
                                    pid_t pid;
                                    int stat;
                                    while((pid = waitpid(-1, &stat, WNOHANG)) > 0)
                                    {
                                        for(int i = 0; i < m_process_number; i++)
                                        {
                                            if(m_sub_process[i].m_pid == pid)
                                            {
                                                printf("child %d join\n", i);
                                                close(m_sub_process[i].m_pipefd[0]);
                                                m_sub_process[i].m_pid = -1;
                                            }
                                        }
                                    }
                                    m_stop = true;
                                    for(int i = 0; i < m_process_number; i++)
                                    {
                                        if(m_sub_process[i].m_pid != -1)
                                            m_stop =false;
                                    }
                                    break;
                                }
                            case SIGTERM:
                            case SIGINT:
                                {
                                    printf("kill all the child now\n");
                                    for(int i = 0; i < m_process_number; i++)
                                    {
                                        int pid = m_sub_process[i].m_pid;
                                        if(pid != -1)
                                            kill(pid, SIGTERM);
                                    }
                                    break;
                                }
                            default:
                                break;
                        }
                    }
                }
            }
            else
                continue;
         }
    }
    close(m_epollfd);
}
#endif
