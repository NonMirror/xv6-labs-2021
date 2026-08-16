#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/param.h"

int main(int argc, char *argv[]) {
  if (argc > MAXARG) {
    fprintf(2, "to many args\n");
    exit(0);
  }

  int cmd_argc_by_xargs = argc - 1;

  char *cmd = argv[1];
 
  char *cmd_argv[MAXARG];
  for (int i = 0; i < cmd_argc_by_xargs; ++i) {
    cmd_argv[i] = argv[1+i];
  }

  char buf[512], *p_start, *p_current;
  p_current = buf; p_start = buf;
  int idx = 0;
  while (read(0, p_current++, 1) == 1) {
    if (*(p_current-1) != '\n') {
      if (*(p_current-1) == ' ') {
        cmd_argv[cmd_argc_by_xargs + idx] = p_start;
        idx++;
        *(p_current-1) = '\0';
        p_start = p_current;
      }
      continue;
    }
    if (p_start != p_current-1) {
      cmd_argv[cmd_argc_by_xargs + idx] = p_start;
      idx++;
      *(p_current-1) = '\0';
    }
    cmd_argv[cmd_argc_by_xargs + idx] = 0;

    if (fork() == 0) {
      exec(cmd, cmd_argv);
      exit(0);
    } else {
      wait(0);
      idx = 0;
      p_current = buf;
      p_start = buf;
    }
  }
  

  exit(0);
}

