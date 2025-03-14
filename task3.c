#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

const size_t write_size = 64 * 1024;
const size_t buf_len = 1024;

void check(int res, const char *msg)
{
    if (res < 0)
    {
        perror(msg);
        exit(1);
    }
}

void my_write(int fd, const char *buf, size_t l)
{
    size_t w = 0;

    while (l > 0)
    {
        size_t s = l > write_size ? write_size : l;

        ssize_t written = write(fd, buf + w, s);
        check(written, "write to fd error! ");

        w += written;
        l -= written;
    }
}

int main(int argc, char *argv[])
{
    check(argc - 2, "No arguments error! ");

    int pipefd[2];
    check(pipe(pipefd), "pipe creating error! ");

    pid_t pid = fork();
    check(pid, "fork error! ");

    if (pid > 0)
    {
        close(pipefd[0]);

        printf("Parent pid = %d, child pid = %d, sending...\n", getpid(), pid);

        for (int i = 1; i < argc; ++i)
        {
            my_write(pipefd[1], argv[i], strlen(argv[i]));

            my_write(pipefd[1], "\n", 1);
        }

        check(close(pipefd[1]), "close write pipe error! ");

        wait((int *)0);

        exit(0);
    }

    else // if (pid == 0)
    {
        check(close(pipefd[1]), "close write pipe error! ");

        char buf[buf_len];
        ssize_t r;

        while ((r = read(pipefd[0], buf, sizeof(buf))) > 0)
        {
            my_write(1, buf, r);
        }

        check(r, "read pipe error! ");

        check(close(pipefd[0]), "close read pipe error! ");
        exit(0);
    }
}
