#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void sieve(int read_fd) __attribute__((noreturn));

void sieve(int read_fd) {
  int prime;
  if (read(read_fd, &prime, sizeof(int)) == 0 || prime > 280) {
    close(read_fd);
    exit(0);
  }

  printf("prime %d\n", prime);

  int next_fd[2];
  pipe(next_fd);

  if (fork() == 0) {
    close(read_fd);
    close(next_fd[1]);
    sieve(next_fd[0]);
  } else {
    close(next_fd[0]);

    int num;
    while (read(read_fd, &num, sizeof(int)) > 0) {
      if (num % prime != 0 && num <= 280) {
        write(next_fd[1], &num, sizeof(int));
      }
    }

    close(read_fd);
    close(next_fd[1]);

    wait(0);
  }

  exit(0);
}

int
main(int argc, char *argv[])
{
  int fd[2];
  pipe(fd);

  if (fork() == 0) {
    close(fd[1]);
    sieve(fd[0]);
  } else {
    close(fd[0]);
    for (int i = 2; i <= 280; ++i) {
      write(fd[1], &i, sizeof(i));
    }
    close(fd[1]);
    wait(0);
  }
  exit(0);
}
