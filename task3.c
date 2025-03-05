#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

const int write_size = 64 * 1024;
const int buf_len = 1024;

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Program needs arguments!\n");
        exit(1);
    }

    int pipefd[2];
    if (pipe(pipefd) == -1)
    {
        fprintf(stderr, "Pipe creating error!\n");
        exit(1);
    }

    pid_t pid = fork();
    if (pid < 0)
    {
        fprintf(stderr, "Fork error!\n");
        exit(1);
    }

    else if (pid > 0)
    {
        close(pipefd[0]);

        printf("Parent pid = %d, child pid = %d, sending...\n", getpid(), pid);

        for (int i = 1; i < argc; ++i)
        {
            int l = strlen(argv[i]);
            int w = 0;

            while (l > 0)
            {
                int s = l > write_size ? write_size : l;

                int written = write(pipefd[1], argv[i] + w, s);
                if (written == -1)
                {
                    fprintf(stderr, "Write pipe error!\n");
                    exit(1);
                }

                w += written;
                l -= written;
            }

            if (write(pipefd[1], "\n", 1) != 1)
            {
                fprintf(stderr, "Write pipe error!\n");
                exit(1);
            }
        }

        int ret = close(pipefd[1]);

        if (ret < 0)
        {
            fprintf(stderr, "Close write pipe error!\n");
            exit(1);
        }
        else
            wait((int *)0);
        
        exit(0);
    }

    else if (pid == 0)
    {
        close(pipefd[1]);

        char buf[buf_len];
        int r;
        
        while ((r = read(pipefd[0], buf, sizeof(buf))) > 0)
        {
            if (write(1, buf, r) != r) //C fprintf иногда бажит)
            {
                fprintf(stderr, "Write to stdout error!\n");
                exit(1);
            }
        }

        if (r == -1)
        {
            fprintf(stderr, "Read pipe error!\n");
            exit(1);
        }

        close(pipefd[0]);
        exit(0);
    }
}
