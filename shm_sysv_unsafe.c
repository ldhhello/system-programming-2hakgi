// shm_sysv_unsafe.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define IPC_KEY_FILENAME "/proc"
#define IPC_KEY_PROJ_ID 100
#define MAX_TRY 1000000

struct shared_data
{
    int counter;
};

void do_sub(struct shared_data* info)
{
    int i;
    // shared memory에 데이터 쓰기
    for (i=0; i<MAX_TRY; i++)
    {
        info->counter--;
        printf("counter: %d\n", info->counter);
    }
}
void do_add(struct shared_data* info)
{
    int i;
    // shared memory에 데이터 쓰기
    for (i=0; i<MAX_TRY; i++)
    {
        info->counter++;
        printf("counter: %d\n", info->counter);
    }
}

//./shm_sysv_unsafe
int main(int argc, char** argv)
{
    key_t key;
    int shmid;
    struct shared_data* info;
    int pid;

    key = ftok(IPC_KEY_FILENAME, IPC_KEY_PROJ_ID);
    if (key == -1)
    {
        perror("ftok()");
        return -1;
    }

    // 공유 메모리 생성
    shmid = shmget(key, sizeof(struct shared_data), 0644 | IPC_CREAT);;

    if (shmid == -1)
    {
        perror("shmget()");
        return -1;
    }

    // shared memory를 process 주소 공간에 attach
    // 여러분 이거 타입 캐스팅 안 해도 오류 안 나는 거 아시나요?
    info = (struct shared_data*) shmat(shmid, NULL, 0);

    if (info == (void*)-1)
    {
        perror("shmat()");
        return -1;
    }

    pid = fork();

    if (pid > 0)
    {
        do_sub(info);

        wait(NULL);

        printf("final counter: %d\n", info->counter);

        if (shmdt(info) == -1)
        {
            perror("shmdt()");
            return -1;
        }

        // 공유 메모리 제거
        shmctl(shmid, IPC_RMID, NULL);
    }
    else if(pid == 0)
    {
        do_add(info);
    }
    else
    {
        perror("fork()");
        return -1;
    }

    return 0;
}
