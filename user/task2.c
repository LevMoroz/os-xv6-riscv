#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

const int write_size = 64;

int main(int argc, char* argv[])
{
  if (argc < 2)
  {
    fprintf(2, "Program needs arguments!\n");
    exit(1);
  }


  int pipefd[2];
  if (pipe(pipefd) < 0)
  {
    fprintf(2, "pipe creating error!\n");
    exit(1);
  }


  int pid = fork();
  if (pid < 0)
  {
    fprintf(2, "fork error!\n");
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
          fprintf(2, "write pipe error!\n");
          exit(1);
        }

        w += written;
        l -= written;
      }

      if (write(pipefd[1], "\n", 1) != 1)
      {
        fprintf(2, "write pipe error!\n");
        exit(1);
      }
    }

    int ret = close(pipefd[1]);

    if(ret < 0)
    {
      fprintf(2, "close write pipe error!\n");
      exit(1);
    }
    else
      wait((int*) 0);

    exit(0);
  }

  else if (pid == 0)
  {
    close(pipefd[1]);

    close(0);
    if (dup(pipefd[0]) == -1)
    {
      fprintf(2, "dup error!\n");
      exit(1);
    }
    close(pipefd[0]);

    char* arg[] = {"/wc", 0};
    exec("/wc", arg);

    fprintf(2, "Error: exec of wc failed!\n");
    exit(1);
  }
}
