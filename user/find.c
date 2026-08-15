#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

char* fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
  return buf;
}

void find(char* path, char* target) {
  char buf[512], *p;
  int fd;
  struct stat st;
  struct dirent de;

  if ((fd = open(path, 0)) < 0) {
    fprintf(2, "cannot open path: %s\n", path);
    exit(0);
  }

  if (fstat(fd, &st) < 0) {
    fprintf(2, "cannot stat path\n");
    exit(0);
  }

// claim st.type == T_FILE
  if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
    printf("ls: path too long\n");
    close(fd);
    return;
  }

  strcpy(buf, path);
  p = buf + strlen(buf);
  *p++ = '/';
  while (read(fd, &de, sizeof(de)) == sizeof(de)) {
    if (de.inum == 0)
      continue;
    if (!strcmp(de.name, ".") || !strcmp(de.name, "..")) {
      continue;
    }
    memmove(p, de.name, DIRSIZ);
    p[DIRSIZ] = 0;
    if (stat(buf, &st) < 0) {
      printf("ls: cannot stat %s\n", buf);
      continue;
    }
    switch (st.type) {
      case T_FILE:
      if (!strcmp(target, de.name)) {
          printf("%s\n", buf);
      }
      break;

      case T_DIR:
      if (!strcmp(buf, ".") || !strcmp(buf, ".."))
        break;
      find(buf, target);
    }
  }
}

int main(int argc, char* argv[]) {
  if (argc != 3) {
    exit(0);
  }

  find(argv[1], argv[2]);

  exit(0);
}
