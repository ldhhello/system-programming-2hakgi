// fifo.c

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define FIFO_FILE "./ldh-fifofile"

char buf[1024];

int do_write()
{
    // 파이프 파일이 있으면 삭제
    unlink(FIFO_FILE);

    printf("before mkfifo\n");
    if(mkfifo(FIFO_FILE, 0644) != 0)
    {
        perror("mkfifo() error");
        return -1;
    }

    printf("before open\n");
    int fd = open(FIFO_FILE, O_WRONLY);
    if(fd < 0)
    {
        perror("open() error");
        return -1;
    }

    printf("before write\n");
    write(fd, buf, strlen(buf));

    close(fd);
    return 0;
}

int do_read()
{
    printf("before open\n");

    int fd = open(FIFO_FILE, O_RDONLY);
    if(fd < 0)
    {
        perror("open() error");
        return -1;
    }

    printf("before read()\n");
    int sz = read(fd, buf, sizeof buf);

    if(sz <= 0)
    {
        perror("read() error");
        return -1;
    }

    printf("message: %s\n", buf);

    close(fd);
    return 0;
}

// ./fifo w "hello world"
// ./fifo r
int main(int argc, char** argv)
{
    if(argc < 2)
    {
        printf("Usage: %s (w or r)\n", argv[0]);
        return -1;
    }

    if(strcmp(argv[1], "w") == 0)
    {
        // writer
        strncpy(buf, argv[2], strlen(argv[2]));
        do_write();
    }
    else if(strcmp(argv[1], "r") == 0)
    {
        // reader
        do_read();
    }

    return 0;
}