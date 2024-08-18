#include <stdio.h>
#include <stdlib.h>

int main() {
  FILE* file = fopen("testfile", "r");

  char* line_read = NULL;
  size_t num_char_read;

  for (int i = 0; i < 3; i++) {
    line_read = NULL;
    ssize_t num_char_read_2 = getline(&line_read, &num_char_read, file);
    printf("%d\n", num_char_read_2);
    printf("%s", line_read);
    free(line_read);
  }
  
  return 0;
}
