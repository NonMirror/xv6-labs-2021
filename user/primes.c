#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void primes(int in[]) {
  int prime, num;
  if (read(in[0], &prime, sizeof(prime)) != sizeof(prime)) {
    fprintf(1, "pid %d: EOF\n", getpid());
    close(in[0]);
    return;
  }
  printf("prime %d\n", prime);

  int out[2]; pipe(out);
  
  if (fork() == 0) {
    close(out[1]); close(in[0]);
    primes(out);
    close(out[0]);
    exit(0);
  } else {
    close(out[0]);
    while (read(in[0], &num, sizeof(num)) == sizeof(num)) {
      if (num % prime != 0) {
        write(out[1], &num, sizeof(num));
      }
    }
    close(out[1]); close(in[0]);
    wait(0);
    exit(0);
  }
}

int main() {
  int start = 2, end = 35;
  int p[2];
  pipe(p);

  if (fork() == 0) {
    close(p[1]);
    primes(p);
    close(p[0]);
    exit(0);
  } else {
    close(p[0]);
    for (int n = start; n <= end; ++n) {
      write(p[1], &n, sizeof(n));
    }
    close(p[1]);
    wait(0);
    exit(0);
  }
}
