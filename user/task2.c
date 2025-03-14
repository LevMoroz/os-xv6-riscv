#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

const int write_size = 64;

void check(int res, const char *msg)
{
  if (res < 0)
  {
    fprintf(2, "%s error!\n", msg);
    exit(1);
  }
}

int main(int argc, char *argv[])
{
  check(argc - 2, "No arguments");

  int pipefd[2];
  check(pipe(pipefd), "pipe creating");

  int pid = fork();
  check(pid, "fork");

  if (pid > 0)
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
        check(written, "write pipe");

        w += written;
        l -= written;
      }

      int res;
      while ((res = write(pipefd[1], "\n", 1)) == 0)
        ;
      check(res, "write pipe");
    }

    check(close(pipefd[1]), "close write pipe");

    wait((int *)0);

    exit(0);
  }

  else // if (pid == 0)
  {
    check(close(pipefd[1]), "close write pipe");

    check(close(0), "close stdin");

    check(dup(pipefd[0]), "dup");

    check(close(pipefd[0]), "close read pipe");

    char *arg[] = {"/wc", 0};
    exec("/wc", arg);

    fprintf(2, "Error: exec of wc failed!\n");
    exit(1);
  }
}
