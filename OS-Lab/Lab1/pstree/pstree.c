#include <stdio.h>
#include <assert.h>
#include <dirent.h>
#include <string.h>
#include <malloc.h>
#include <stdbool.h>

#define MAX_PROC_NUMBER 600

char *get_proc_name(int number);
void get_proc_pid_ppid(char *number, int *pid, int *ppid);

struct PROC
{
  int pid;
  int spid[30];
  int cnt;
};

struct PROC procs[MAX_PROC_NUMBER];

int table[MAX_PROC_NUMBER];
int cnt;
void show_ptree_helper(int pid, int depth, int number);

int is_all_digit(const char *name)
{
  while (*name)
  {
    if (*name < '0' || *name > '9')
    {
      return 0;
    }
    name++;
  }
  return 1;
}

int b_search(int pid)
{
  for (int i = 0; i < cnt; i++)
  {
    if (table[i] == pid)
    {
      return i;
    }
  }
  return -1;
}

void show_ptree()
{
  show_ptree_helper(1, 0, 0);
}

void show_ptree_helper(int pid, int depth, int number)
{
  int id = b_search(pid);
  struct PROC proc = procs[id];
  for (int i = 0; i < depth - 1; i++)
  {
    printf("    ");
  }
  printf("|----");
  char *name = get_proc_name(pid);
  printf("--%s", name);
  for (int i = 0; i < proc.cnt; i++)
  {
    int spid = proc.spid[i];

    show_ptree_helper(spid, depth + 1, i);
  }

  return;
}

void add_edge(int ppid, int pid)
{

  if (b_search(ppid) == -1 && ppid != 0)
  {
    struct PROC pproc;
    pproc.pid = ppid;
    pproc.cnt = 0;
    table[cnt] = ppid;
    procs[cnt++] = pproc;
  }
  if (b_search(pid) == -1)
  {
    struct PROC proc;
    proc.pid = pid;
    proc.cnt = 0;
    table[cnt] = pid;
    procs[cnt++] = proc;
  }
  if (ppid != 0)
  {
    int id = b_search(ppid);
    struct PROC *pproc = &procs[id];
    pproc->spid[pproc->cnt++] = pid;
  }
}

void print_all_proc()
{
  for (int i = 0; i < cnt; i++)
  {
    char *name = get_proc_name(table[i]);
    printf("proc id = %d,name = %s", table[i], name);
    free(name);
  }
}

void get_all_proc()
{
  DIR *d;
  struct dirent *dir;
  d = opendir("/proc");
  
  if (d)
  {
    while ((dir = readdir(d)) != NULL)
    {
      if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0)
      {
        continue;
      }
      if (dir->d_type == 4 && is_all_digit(dir->d_name) == 1) // its a proc's dir
      {
        int pid = 0;
        int ppid = 0;
        get_proc_pid_ppid(dir->d_name, &pid, &ppid);
        //printf("1");
        add_edge(ppid, pid);
        
      }
      //return;
        
    }
    
    
  }
  else
  {
    printf("Can't open /proc!");
  }
  
  closedir(d);
  
}

void get_proc_pid_ppid(char *number, int *pid, int *ppid)
{
  char abs_path[50];
  sprintf(abs_path, "/proc/%s/stat", number);
  FILE *fp = fopen(abs_path, "r");
  if (!fp)
    goto release;
  fscanf(fp, "%d %*s %*c %d", pid, ppid);
release:
  if (fp)
    fclose(fp);
  return;
}

char *get_proc_name(int number) // this function should return the proc's name of the current dir
{
  char abs_path[20];
  sprintf(abs_path, "/proc/%d/comm", number);
  FILE *fp = fopen(abs_path, "r");
  if (!fp)
    goto release;
  char *name = (char *)malloc(64 * sizeof(name));
  fgets(name, 64, fp);
release:
  if (fp)
    fclose(fp);
  return name;
}

int _pstree(int argc, char *argv[])
{
  for (int i = 0; i < argc; i++)
  {
    assert(argv[i]);
    printf("argv[%d] = %s\n", i, argv[i]);
  }
  assert(!argv[argc]);
  
  get_all_proc();
  show_ptree();
  return 0;
}