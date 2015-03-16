.PHONY:clean
CC	=	gcc
CFLAGS	=	-Wall -g
BIN		=	FtpServer
OBJS	=	src/main.o src/sysutil.o src/session.o src/strutil.o \
	 src/ftp_nobody.o src/ftp_proto.o src/configure.o \
	 src/parse_conf.o src/command_map.o src/trans_data.o \
	 src/priv_sock.o src/priv_ops.o src/trans_ctrl.o \
	 src/hash.o src/ftp_assist.o
LIB		=	-lcrypt
$(BIN):$(OBJS)
	$(CC) $(CFLAGS) $^ -o bin/$@ $(LIB)
	rm -f src/*.o
%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@ -I./include
clean:
	rm -f src/*.o bin/$(BIN)
