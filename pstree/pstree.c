#include <stdio.h>
#include <assert.h>
#include <dirent.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

int getMaxPid() {
  char *line = NULL;
  size_t length = 0;
  FILE *f = fopen("/proc/sys/kernel/pid_max", "r");
  if (f) {
    int r = 0;
    if(getline(&line, &length, f) != -1) {
      printf("line is: %s\n", line);
      r = atoi(line);
    }
    fclose(f);
    return r;
  } else {
    return 0;
  }
}

int isNumber(const char* str) {
  while(*str != '\0') {
    if(!isdigit(*str)) 
      return 0;
    str++;
  }
  return 1;
}

struct Process
{
  int pid;
  char comm[1024];
  char state;
  int ppid;
};


int main(int argc, char *argv[]) {
  for (int i = 0; i < argc; i++) {
    assert(argv[i]);
    printf("argv[%d] = %s\n", i, argv[i]);
  }

  int pid_max = getMaxPid();
  printf("max pid is:%d\n", pid_max);
  if(pid_max <=0) {
    printf("pid max is less or equal zero, system error!");
    return EXIT_SUCCESS;
  }
  struct Process *processList = malloc(pid_max * sizeof(struct Process));
  for(long i =0; i < pid_max; ++i) {
    processList[i].pid = -1;
  }

  const char *path = "/proc";
  DIR *d = opendir(path);
  struct dirent *dir;
  if(d) {
    while((dir = readdir(d)) != NULL) {
     if(isNumber(dir->d_name)) {
      int dname = atoi(dir->d_name);
      char filepath[1000];
      sprintf(filepath, "/proc/%d/stat", dname);
      FILE *f = fopen(filepath, "r");
      int pid;
      char comm[1024];
      char state;
      int ppid;
      fscanf(f, "%d %s %c %d", &pid, comm, &state, &ppid);
      printf("%d %s %c %d\n", pid, comm, state, ppid);
      processList[dname].pid = pid;
      strncpy(processList[dname].comm, comm, sizeof(comm));
      processList[dname].state = state;
      processList[dname].ppid = ppid;
     }
    }
    closedir(d);
  } else {
    printf("can't open path: %s\n", path);
  }
  free(processList);
  assert(!argv[argc]);
  return EXIT_SUCCESS;
}
