#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
  int pid = fork();

  if (pid < 0)
  {
    fprintf(2, "fork error!\n");
    exit(1);
  }
  else if (pid > 0)
  {
    printf("Parent pid = %d, child pid = %d, waiting...\n", getpid(), pid);
    int status, cpid;
    cpid = wait(&status);

    if (cpid == -1)
    {
      fprintf(2, "fork error before, no child!\n");
      exit(1);
    }

    printf("Parent pid = %d, child pid = %d exit with status %d\n", getpid(), cpid, status);

    exit(0);
  }
  else if (pid == 0)
  {
    sleep(128);
    exit(1);
  }
}
