#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

const int buf_len = 100;

struct atoi_s
{
  int num;
  char* next;
};

struct atoi_s my_atoi(char* input) // Нормальный atoi + exit
{
  int sgn = 1;

  if (*input == '\0')
  {
    fprintf(2, "Not enough numbers to convert!\n");
    exit(1);
  }

  while (*input == ' ')
    ++input;

  if (*input == '+')
    ++input;

  if (*input == '-')
  {
    sgn = -1;
    ++input;
  }

  int num = 0;

  if (*input < '0' || *input > '9')
  {
    fprintf(2, "There is no number!\n");
    exit(1);
  }

  while (*input != ' ' && *input != '\0')
  {
    if (*input < '0' || *input > '9')
    {
      fprintf(2, "This is not a number!\n");
      exit(1);
    }

    num = num * 10 + (*input - '0');
    ++input;
  }

  struct atoi_s a;
  a.num = sgn * num;
  a.next = input;
  return a;
}

int main(int argc, char *argv[])
{
  char buf[buf_len];
  printf("Write two numbers with whitespace to add:\n");

  int i = 0;
  for (;;)
  {
    int res = read(0, &buf[i], sizeof(char));

    if (res == -1)
    {
      fprintf(2, "Some read error\n");
      exit(1);
    }

    if (res == 0 || buf[i] == '\n' || buf[i] == '\r' || buf[i] == '\0')
    {
      buf[i] = '\0';
      break;
    }

    ++i;
    if (i == buf_len)
    {
      fprintf(2, "Buffer is overflowed (too big numbers?)\n");
      exit(1);
    }
  }

  printf("|%s|\n", buf);

  struct atoi_s n1 = my_atoi(buf);
  int num2 = my_atoi(n1.next).num;

  printf("Result: %d\n", add(n1.num, num2));

  exit(0);
}
