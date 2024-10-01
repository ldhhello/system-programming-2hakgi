// socket_stream.c
// Domain : UNIX Domain Socket
// Type : Datagram

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCK_PATH "ldh_sock_file"
#define MAX_SIZE 1024

// socket -> bind -> listen -> accept -> send/recv -> close
int do_server()
{
    int sockfd;
    struct sockaddr_un addr;
    char buf[MAX_SIZE];
    int ret;

    if((sockfd = socket(AF_UNIX, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket()");
        return -1;
    }
    printf("socket() 성공\n");

    memset(&addr, 0, sizeof addr);
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCK_PATH, sizeof(addr.sun_path) - 1);

    // 소켓을 닫은 후 바로 재ㅎ사용할 수 없도록 TIME_WAIT 상태를
    unlink(SOCK_PATH);

    if(bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
        perror("bind()");
        close(sockfd);
        return -1;
    }
    printf("bind() 성공\n");

    memset(buf, 0, sizeof buf);

    if ((ret = recvfrom(sockfd, buf, sizeof buf, 0, NULL, NULL)) < 0)
    {
        perror("recvfrom()");
        close(sockfd);
        return -1;
    }

    printf("message from client: %s, ret: %d\n", buf, ret);

    close(sockfd);

    return 0;
}
// socket -> connect -> send/recv -> close
int do_client()
{
    int sockfd;
    struct sockaddr_un addr;
    //char buf[MAX_SIZE];
    int ret;

    if((sockfd = socket(AF_UNIX, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket()");
        return -1;
    }
    printf("socket() 성공\n");

    memset(&addr, 0, sizeof addr);
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCK_PATH, sizeof(addr.sun_path) - 1);

    char* msg = "hello, server";
    if ((ret = sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr*)&addr, sizeof addr)) < 0)
    {
        perror("send()");
        close(sockfd);
        return -1;
    }

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
