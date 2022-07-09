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

void constructProcess(const char* stat, struct Process *processList) {
  char temp[128];
  int index = 0;
  memset(temp, 0, sizeof(temp));
  const char *p = stat;
  while (*p != '\0')
  {
    if(*p != ' ') {
      temp[index++] = *p;
      ++p;
    } else {
      break;
    }
  }
  
  int pid = atoi(temp);
  index = 0;
  memset(temp, 0, sizeof(temp));
  p += 2;
  while(*p != ')') {
    temp[index++] = *p;
    ++p;
  }
  char state;
  int ppid;
  sscanf(p+2, "%c %d", &state, &ppid);
  processList[pid].pid = pid;
  memcpy(processList[pid].comm, temp, strlen(temp));
  processList[pid].state = state;
  processList[pid].ppid = ppid;
}


int main(int argc, char *argv[]) {
  for (int i = 0; i < argc; i++) {
    assert(argv[i]);
    printf("argv[%d] = %s\n", i, argv[i]);
  }

  int pid_max = getMaxPid();
  if(pid_max <=0) {
    printf("pid max is less or equal zero, system error!");
    return EXIT_SUCCESS;
  }
  

  const char *path = "/proc";
  DIR *d = opendir(path);
  struct dirent *dir;
  if(d) {
    struct Process *processList = malloc(pid_max * sizeof(struct Process));
    for(long i =0; i < pid_max; ++i) {
      processList[i].pid = -1;
    }
    while((dir = readdir(d)) != NULL) {
      if(isNumber(dir->d_name)) {
        int dname = atoi(dir->d_name);
        char filepath[1000];
        sprintf(filepath, "/proc/%d/stat", dname);
        FILE *f = fopen(filepath, "r");
        char *line = NULL;
        size_t length = 0;
        if(getline(&line, &length, f) != -1) {
          constructProcess(line, processList);
        }
      }
    }
    closedir(d);
    for(int i = 0; i < pid_max; ++i) {
      if(processList[i].pid != -1) {
        printf("%d %s %c %d\n", processList[i].pid, processList[i].comm, processList[i].state, processList[i].ppid);
      }
    }
    free(processList);
  } else {
    printf("can't open path: %s\n", path);
    return EXIT_FAILURE;
  }
  
  assert(!argv[argc]);
  return EXIT_SUCCESS;
}
