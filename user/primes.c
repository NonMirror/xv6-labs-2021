#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
void primes(int *in);

void primes(int* in) {
  int p, n;
  int r = read(in[0], &p, sizeof(p));
  if (r != sizeof(p)) {
    close(in[0]);
    fprintf(2, "pid %d: EOF\n", getpid());
    return;
  }
  printf("prime %d\n", p);

  int out[2];
  pipe(out);
  if (fork() == 0) {
    close(in[0]);
    close(out[1]);
    primes(out);
    exit(0);
  } else {
    close(out[0]);
    while (read(in[0], &n, sizeof(n)) == sizeof(n)) {
      if (n % p != 0) {
        write(out[1], &n, sizeof(n));
      }
    }
    close(in[0]);
    close(out[1]);
    wait(0);
    exit(0);
  }
}

int main() {
  int start = 2, end = 35;
  int fd[2];
  pipe(fd);

  if (fork() == 0) {
    close(fd[1]);
    primes(fd);
  } else {
    close(fd[0]);
    for (int i = start; i <= end; ++i) {
      write(fd[1], &i, sizeof(i));
    }
    close(fd[1]);
    wait(0);
    exit(0);
  }
  return 0;
}

