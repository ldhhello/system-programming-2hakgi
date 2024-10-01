#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>

#define POSIX_MQ_NAME "/posixmq"
#define MAX_SIZE 1024
int get_mqd()
{
    mqd_t mqd;
    // mqd_t mq_open(const char* name, int oflag, mode_t mode, struct mq_attr* attr)
    // 마지막 인자를 NULL로 설정 시 시스템 티폴트 값 설정
    mqd = mq_open(POSIX_MQ_NAME, O_RDWR | O_CREAT, 0644, NULL);
    if(mqd == -1)
    {
        perror("mq_open()");
        return -1;
    }

    // MQ 속성 값 확인하기
    struct mq_attr attr;
    memset(&attr, 0, sizeof (attr));
    if (mq_getattr(mqd, &attr) != 0)
    {
        perror("mq_getattr()");
        return -1;
    }

    printf("mq_flags : %ld\n", attr.mq_flags);
    printf("mq_maxmsg : %ld\n", attr.mq_maxmsg);
    printf("mq_msgsize : %ld\n", attr.mq_msgsize);
    printf("mq_mq_curmsgs : %ld\n", attr.mq_curmsgs);

    return mqd;
}

int do_send()
{
    mqd_t mqd;
    char buf[MAX_SIZE];
    int ret;

    mqd = get_mqd();

    if (mqd == -1)
    {
        perror("get_mqd()");
        return -1;
    }

    memset(buf, 0, sizeof(buf));
    snprintf(buf, sizeof(buf), "hello, pid %d", getpid());

    ret = mq_send(mqd, buf, strlen(buf), 0);
    if (ret == -1)
    {
        perror("mq_send()");
    }

    mq_close(mqd);
    return ret;
}

int do_recv()
{
    mqd_t mqd;
    char buf[8192];
    int ret;

    mqd = get_mqd();

    if (mqd == -1)
    {
        perror("get_mqd()");
        return -1;
    }

    memset(buf, 0, sizeof(buf));

    ret = mq_receive(mqd, buf, sizeof(buf), NULL);
    if (ret == -1)
    {
        perror("mq_receive()");
    }
    else
    {
        printf("received message: %s\n", buf);
    }

    return 0;
}

// ./mq_posix s
// ./mq_posix r
int main(int argc, char** argv)
{
    int ret;

    if (argc < 2)
    {
        printf("Usage: %s (s or r)\n", argv[0]);
        return -1;
    }

    if(strcmp(argv[1], "s") == 0)
    {
        ret = do_send();
    }
    else if(strcmp(argv[1], "r") == 0)
    {
        ret = do_recv();
    }
    else
    {
        printf("Usage: %s (s or r)\n", argv[0]);
        return -1;
    }

    return ret;
}