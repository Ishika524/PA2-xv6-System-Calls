#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define MAX_PROCS 64

struct proc_info procs[MAX_PROCS];
int nprocs = 0;
int show_pids = 0;
int show_mem = 0;

void print_node(struct proc_info *p, int depth) {
  // Print indentation: depth 0 has no prefix, depth > 0 has (depth-1)*3 spaces followed by |- 
  if (depth > 0) {
    for (int i = 0; i < depth - 1; i++) {
      printf("   ");
    }
    printf("|- ");
  }

  // Print process name
  printf("%s", p->name);

  // Optional PID
  if (show_pids) {
    printf("(%d)", p->pid);
  }

  // Optional Memory Size
  if (show_mem) {
    printf(" [%dB]", (int)p->sz);
  }

  printf("\n");
}

void print_tree(int pid, int depth) {
  // Find node with this pid
  struct proc_info *curr = 0;
  for (int i = 0; i < nprocs; i++) {
    if (procs[i].pid == pid) {
      curr = &procs[i];
      break;
    }
  }

  if (curr == 0) return;

  // Print current node
  print_node(curr, depth);

  // Recursively print all children
  for (int i = 0; i < nprocs; i++) {
    if (procs[i].ppid == pid && procs[i].pid != pid) {
      print_tree(procs[i].pid, depth + 1);
    }
  }
}

int main(int argc, char *argv[]) {
  // Parse command line flags: -p, -m, -pm, -mp
  for (int i = 1; i < argc; i++) {
    char *arg = argv[i];
    if (arg[0] == '-') {
      for (int j = 1; arg[j] != '\0'; j++) {
        if (arg[j] == 'p') {
          show_pids = 1;
        } else if (arg[j] == 'm') {
          show_mem = 1;
        }
      }
    }
  }

  nprocs = getprocs(procs, MAX_PROCS);
  if (nprocs < 0) {
    printf("pstree: failed to get process info\n");
    exit(1);
  }

  // Root of the tree is init (PID 1)
  print_tree(1, 0);

  exit(0);
}