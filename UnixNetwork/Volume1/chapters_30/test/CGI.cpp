#include "process.h"

/* 用于处理客户的CGI请求 */
class cgi_conn
{
    private:
        static const int BUFFER_SIZE = 1024;
        static int m_epollfd;
        int m_sockfd;
        struct sockaddr_in m_address;
        char m_buf[BUFFER_SIZE];
        /* 标记读缓冲区中已经读入的客户数据的最后一个自己的下一个位置 */
        int m_read_next;
    public:
        cgi_conn(){}
        ~cgi_conn(){}

        /* 初始化客户连接，清空缓冲区 */
        void init(int epollfd, int sockfd, const struct sockaddr_in &cliaddr)
        {
            m_epollfd = epollfd;
            m_sockfd = sockfd;
            m_address = cliaddr;
            memset(m_buf, '\0', BUFFER_SIZE);
            m_read_next = 0;
        }

        void process()
        {
            int idx = 0;
            int ret = -1;

            while(1)
            {
                idx = m_read_next;
                ret = recv(m_sockfd, m_buf+idx, BUFFER_SIZE-1-idx, 0);
                if(ret < 0)
                {
                    if(errno != EAGAIN)
                        remove(m_epollfd, m_sockfd);
                    break;
                }
                else if(ret == 0)
                {
                    remove(m_epollfd, m_sockfd);
                    break;
                }
                else
                {
                    m_read_next += ret;
                    printf("user content is: %s\n", m_buf);
                    /* 若遇到字符“\r\n”，则开始处理客户请求 */
                    for(; idx < m_read_next; ++idx)
                    {
                        if((idx >= 1) && (m_buf[idx-1] == '\r') && (m_buf[idx-1] == '\n'))
                            break;
                    }
                    if(idx == m_read_next)
                        continue;
                    m_buf[idx-1] = '\0';

                    char *file_name = m_buf;
                    /* 判断客户要运行的CGI程序是否存在 */
                    if(access(file_name, F_OK) == -1)
                    {
                        remove(m_epollfd, m_sockfd);
                        break;
                    }
                    /* 创建子进程来执行CGI程序 */
                    ret = fork();
                    if(ret == -1)
                    {
                        remove(m_epollfd, m_sockfd);
                        break;
                    }
                    else if(ret > 0)
                    {
                        remove(m_epollfd, m_sockfd);
                        break;
                    }
                    else
                    {
                        /* 子进程将标准输出定向到m_sockfd，并执行CGI程序 */
                        close(STDOUT_FILENO);
                        dup(m_sockfd);
                        execl(m_buf, m_buf, 0);
                        exit(0);
                    }
                }
            }
        }
};


int cgi_conn::m_epollfd = -1;

int main(int argc, char *argv[])
{
    if(argc <= 2)
    {
        printf("usage: %s ip_address port_number\n", basename(argv[0]));
        return 1;
    }

    const char *ip = argv[1];
    int port = atoi(argv[2]);

    int listenfd = socket(PF_INET, SOCK_STREAM, 0);
    assert(listenfd >= 0);

    int ret = 0;
    struct sockaddr_in address;
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &address.sin_addr);
    address.sin_port = htons(port);

    ret = bind(listenfd, (struct sockaddr *)&address, sizeof(address));
    assert(ret != -1);

    ret = listen(listenfd, 5);
    assert(ret != -1);

    /* 创建进程池 */
    processpool<cgi_conn>* pool = processpool<cgi_conn>::create(listenfd);
    if(pool)
    {
        pool->run();
        delete pool;
    }

    close(listenfd);

    return 0;
}
