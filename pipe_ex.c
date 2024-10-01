// pipe_ex.c

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char** argv)
{
    int pipe_fds[2];
    pid_t pid;
    int wstatus;

    if(pipe(pipe_fds) != 0)
    {
        perror("pipe() error!");
        return -1;
    }

    pid = fork();

    if(pid > 0)
    {
        // parent process
        close(pipe_fds[0]);

        printf("학번을 입력하세요 : ");
        int hakbun;
        scanf("%d", &hakbun);

        printf("이름을 입력하세요 : ");
        char name[100];
        scanf("%s", name);

        write(pipe_fds[1], &hakbun, 4);
        write(pipe_fds[1], name, strlen(name) + 1);

        printf("Message sent!!\n");

        close(pipe_fds[1]);

        wait(&wstatus);
    }
    else if(pid == 0)
    {
        // child process
        close(pipe_fds[1]);

        int hakbun;
        char name[100];
        if(read(pipe_fds[0], &hakbun, 4) < 0)
        {
            perror("read() error");
            return -1;
        }
        if(read(pipe_fds[0], name, 100) < 0)
        {
            perror("read() error");
            return -1;
        }
        printf("학번 : %d, 이름 : %s\n", hakbun, name);

        close(pipe_fds[0]);
    }
    else
    {
        perror("oh no! fork() error");

        close(pipe_fds[0]);
        close(pipe_fds[1]);
        return -1;
    }

    return 0;
}
