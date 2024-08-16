#include <unistd.h>
#include <stdio.h>

int main() {
  pid_t pid = fork();

  if (pid == 1) {
    printf("Printed by Parent: Failed to create child\n");
    return 1;
  }

  if (pid == 0) {
    printf("Printed by Child: Hello from Child\n");
  }

  if (pid != 0) {
    printf("Printed by Parent: Hello from Parent\n");
    printf("Printed by Parent: Child's pid is %d\n", pid);
  }

  return 0;
}
  
