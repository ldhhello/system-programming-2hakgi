// socket_stream.c
// Domain : UNIX Domain Socket

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
    int peerfd;
    char buf[MAX_SIZE];
    int ret;

    if((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
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

    char msg[1024];
    while (1)
    {
        if ((ret = recv(peerfd, buf, sizeof buf, 0)) < 0)
        {
            perror("recv()");
            close(sockfd);
            close(peerfd);
            return -1;
        }
        printf("[server] received message: %s\n", buf);
        if (strncmp(buf, "exit", 4) == 0)
        {
            close(sockfd);
            close(peerfd);
            return 0;
        }

        printf("enter your message > ");
        scanf("%s", msg);

        if ((ret = send(peerfd, msg, strlen(msg), 0)) < 0)
        {
            perror("send()");
            close(sockfd);
            close(peerfd);
            return -1;
        }

        if (strncmp(msg, "exit", 4) == 0)
        {
            close(sockfd);
            close(peerfd);
            return 0;
        }
    }

    //close(peerfd);
    //close(sockfd);

    //return 0;
}
// socket -> connect -> send/recv -> close
int do_client()
{
    int sockfd;
    struct sockaddr_un addr;
    char buf[MAX_SIZE];
    int ret;

    if((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
    {
        perror("socket()");
        return -1;
    }
    printf("socket() 성공\n");

    memset(&addr, 0, sizeof addr);
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCK_PATH, sizeof(addr.sun_path) - 1);

    if (connect(sockfd, (struct sockaddr*)&addr, sizeof addr) < 0)
    {
        perror("connect()");
        close(sockfd);
        return -1;
    }
    printf("connect() 성공\n");

    memset(buf, 0, sizeof buf);

    char msg[1024];
    while (1)
    {
        printf("enter your message > ");
        scanf("%s", msg);

        if (strncmp(msg, "exit", 4) == 0)
        {
            close(sockfd);
            return 0;
        }

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
        if (strncmp(buf, "exit", 4) == 0)
        {
            close(sockfd);
            return 0;
        }

        printf("[client] received message: %s\n", buf);
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
