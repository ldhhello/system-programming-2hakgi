// mq_sysv.c

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define IPC_KEY_PATHNAME "/proc"
#define IPC_KEY_PROJ_ID 123

struct msgbuf
{
    long mtype;
    char mtext[1024];
};

int get_msq_id()
{
    key_t key;
    int msq_id;

    key = ftok(IPC_KEY_PATHNAME, IPC_KEY_PROJ_ID);

    if (key == -1)
    {
        perror("ftok() error");
        return -1;
    }
    printf("key: %d\n", key);

    msq_id = msgget(key, 0644 | IPC_CREAT);
    printf("msq_id : %d\n", msq_id);

    if (msq_id == -1)
    {
        perror("msgget() error");
        return -1;
    }

    return msq_id;
}

int do_send()
{
    int msq_id;
    struct msgbuf mbuf;
    int ret;

    msq_id = get_msq_id();

    if(msq_id == -1)
    {
        perror("get_msq_id() error");
        return -1;
    }

    memset(&mbuf, 0, sizeof mbuf);

    mbuf.mtype = 1; // 양수로 지정
    strncpy(mbuf.mtext, "hello world", sizeof(mbuf.mtext) - 1);

    ret = msgsnd(msq_id, &mbuf, sizeof(mbuf.mtext), 0);

    if (ret == -1)
    {
        perror("msgsnd() error");
        return -1;
    }

    return 0;
}

int do_recv() 
{
    int msq_id;
    struct msgbuf mbuf;
    int ret;

    msq_id = get_msq_id();

    if(msq_id == -1)
    {
        perror("get_msq_id() error");
        return -1;
    }

    memset(&mbuf, 0, sizeof(mbuf));

    // 0: 순서대로 수신
    // 양수: mtype과 매치되는 첫 번째 메시지 수신
    // 음수: 절댓값보다 작거나 같은 첫 번째 메시지 수신

    ret = msgrcv(msq_id, &mbuf, sizeof(mbuf.mtext), 1, 0);
    if (ret == -1)
    {
        perror("msgrcv() error");
        return -1;
    }
    printf("received message: %s, mtype: %ld\n", mbuf.mtext, mbuf.mtype);

    return 0;
}

// ./mq_sysv s
// ./mq_sysv r
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