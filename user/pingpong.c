#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
  int p1[2], p2[2];
  pipe(p1); pipe(p2);
  int pid;
  char buf;
  if (fork() == 0) {
    pid = getpid();
    close(p1[1]); close(p2[0]);
    read(p1[0], &buf, 1);
    close(p1[0]);
    printf("%d: received ping\n", pid);
    write(p2[1], &buf, 1);
    close(p2[1]);
    exit(0);
  } else {
    pid = getpid();
    close(p1[0]); close(p2[1]);
    write(p1[1], "b", 1);
    close(p1[1]);
    read(p2[0], &buf, 1); 
    close(p2[0]);
    printf("%d: received pong\n", pid);
    int stat;
    wait(&stat);
    exit(0);
  }
}
