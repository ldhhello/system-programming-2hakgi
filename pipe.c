// pipe.c

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char** argv)
{
    int pipe_fds[2];
    pid_t pid;
    char buf[1024];
    int wstatus;

    memset(buf, 0, sizeof buf);

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

        strncpy(buf, "hello, child", sizeof buf);
        write(pipe_fds[1], buf, strlen(buf) + 1);

        printf("[%d] sent message: %s\n", getpid(), buf);

        close(pipe_fds[1]);

        wait(&wstatus);
    }
    else if(pid == 0)
    {
        // child process
        close(pipe_fds[1]);

        read(pipe_fds[0], buf, sizeof buf);
        printf("[%d] received message: %s\n", getpid(), buf);

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
