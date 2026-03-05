#include "kernel/types.h"
#include "user/user.h"
#include <sys/types.h>

int main() {
  int pipe1[2];
  int pipe2[2];
  char buf;
  pid_t pid;
  if (pipe(pipe1) < 0) {
    fprintf(2, "pipe1 failed\n");
    exit(1);
  }
  if (pipe(pipe2) < 0) {
    fprintf(2, "pipe2 failed\n");
    exit(1);
  }

  pid_t child_pid = fork();
  if (child_pid < 0) {
    fprintf(2, "fork failed\n");
    exit(1);
  }

  if (child_pid == 0) {
    pid = getpid();
    close(pipe1[1]);
    close(pipe2[0]);
    wait(0);
    while (read(pipe1[0], &buf, 1) > 0) {
      fprintf(1, "%d: received ping\n", pid);
      printf("read %c from pipe1\n", buf);
    }
    close(pipe1[0]);
    write(pipe2[1], &buf, 1);
    close(pipe2[1]);
    exit(0);
  }

  if (child_pid > 0) {
    pid = getpid();
    close(pipe1[0]);
    close(pipe2[1]);
    write(pipe1[1], "!", 1);
    close(pipe1[1]);
    wait(0);
    while (read(pipe2[0], &buf, 1) > 0) {
      fprintf(1, "%d: received pong\n", pid);
      printf("read %c from pipe2\n", buf);
    }
    close(pipe2[0]);
    exit(0);
  }

  return 0;
}
