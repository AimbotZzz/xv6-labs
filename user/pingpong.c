#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
  int pipe1[2], pipe2[2];
  char c;

  if (pipe(pipe1) < 0 || pipe(pipe2) < 0) {
    fprintf(2, "pipe creation failed\n");
    exit(1);
  }

  int pid = fork();
  if (pid < 0) {
    fprintf(2, "fork failed\n");
    exit(1);
  }

  if (pid == 0) {
    // Child process
    close(pipe1[1]);
    close(pipe2[0]);

    if (read(pipe1[0], &c, 1) != 1) {
      fprintf(2, "child read from pipe1 failed\n");
      exit(1);
    }
    printf("%d: received ping\n", getpid());

    if (write(pipe2[1], "c", 1) != 1) {
      fprintf(2, "child write to pipe2 failed\n");
      exit(1);
    }

    close(pipe1[0]);
    close(pipe2[1]);

  } else {
    // Parent process
    close(pipe1[0]);
    close(pipe2[1]);

    if (write(pipe1[1], "c", 1) != 1) {
      fprintf(2, "parent write to pipe1 failed\n");
      exit(1);
    }
    if (read(pipe2[0], &c, 1) != 1) {
      fprintf(2, "parent read from pipe2 failed\n");
      exit(1);
    }
    printf("%d: received pong\n", getpid());

    close(pipe1[1]);
    close(pipe2[0]);
  }
  exit(0);
}
