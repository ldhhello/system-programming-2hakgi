// socket_stream_inet.c
// Domain : Internet Domain Socket

#include <asm-generic/socket.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define IP "192.168.5.181"
#define PORT 12345
#define MAX_SIZE 1024

// socket -> bind -> listen -> accept -> send/recv -> close
int do_server()
{
    int sockfd;
    struct sockaddr_in addr;
    int peerfd;
    char buf[MAX_SIZE];
    int ret;
    int optval = 1;

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket()");
        return -1;
    }
    printf("socket() 성공\n");

    memset(&addr, 0, sizeof addr);
    addr.sin_addr.s_addr = INADDR_ANY; // 모든 클라이언트 수신
    addr.sin_port = htons(PORT);

    // 소켓 옵션 설정
    if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval) < 0)
    {
        perror("setsockopt()");
        close(sockfd);
        return -1;
    }

    if(bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
        perror("bind()");
        close(sockfd);
        return -1;
    }
    printf("bind() 성공\n");

    if (listen(sockfd, 3) < 0)
    {
        perror("listen()");
        close(sockfd);
        return -1;
    }
    printf("listen()...\n");

    if ((peerfd = accept(sockfd, NULL, NULL)) < 0)
    {
        perror("accept()");
        close(sockfd);
        return -1;
    }
    printf("accept() 성공\n");

    memset(buf, 0, sizeof buf);

    if ((ret = recv(peerfd, buf, sizeof buf, 0)) < 0)
    {
        perror("recv()");
        close(sockfd);
        close(peerfd);
        return -1;
    }

    printf("message from client: %s, ret: %d\n", buf, ret);

    char* msg = "hi, client";
    if ((ret = send(peerfd, msg, strlen(msg), 0)) < 0)
    {
        perror("send()");
        close(sockfd);
        close(peerfd);
        return -1;
    }

    close(peerfd);
    close(sockfd);

    return 0;
}
// socket -> connect -> send/recv -> close
int do_client()
{
    int sockfd;
    struct sockaddr_in addr;
    char buf[MAX_SIZE];
    int ret;

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket()");
        return -1;
    }
    printf("socket() 성공\n");

    memset(&addr, 0, sizeof addr);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(IP);
    addr.sin_port = htons(PORT);

    if (connect(sockfd, (struct sockaddr*)&addr, sizeof addr) < 0)
    {
        perror("connect()");
        close(sockfd);
        return -1;
    }
    printf("connect() 성공\n");

    memset(buf, 0, sizeof buf);

    char* msg = "hello, server";
    if ((ret = send(sockfd, msg, strlen(msg), 0)) < 0)
    {
        perror("send()");
        close(sockfd);
        return -1;
    }

    if ((ret = recv(sockfd, buf, sizeof buf, 0)) < 0)
    {
        perror("recv()");
        close(sockfd);
        return -1;
    }

    printf("message from server: %s, ret: %d\n", buf, ret);

    close(sockfd);

    return 0;
}

// ./socket_stream server
// ./socket_stream client
int main(int argc, char** argv)
{
    int ret;

    if (argc < 2)
    {
        printf("Usage: %s (server or client)\n", argv[0]);
        return -1;
    }

    if (!strcmp(argv[1], "server"))
    {
        ret = do_server();
    }
    else if (!strcmp(argv[1], "client"))
    {
        ret = do_client();
    }
    else
    {
        printf("Usage: %s (server or client)\n", argv[0]);
        return -1;
    }

    return ret;
}
