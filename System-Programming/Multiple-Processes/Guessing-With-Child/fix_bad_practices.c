#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

// Pass fd direction, not pointer 
int parent_process(int p_to_c, int c_to_p) {
  // Close pipee before, only pass resourses needed
  // Manage pipes as FILE/STREAMs
  // "a" is not needed, just "w"
  FILE* write_to_c = fdopen(p_to_c, "w");
  FILE* read_from_c = fdopen(c_to_p, "r");
  
  // Set up random seq
  srand(0);
  int secret = rand() % 20;
  // Uncomment the following line to see what happens when run out of guesses
  // secret = 100;
  printf("From Parent: Secret is %d\n", secret);

  // Don't put guess in loop, wastes space
  int guess;
  // Should also do for some others
  char* line = NULL;
  size_t len;
  ssize_t read_status;
  
  // Limit to 100 guesses
  int i = 0;
  for (i = 0; i < 100; i++) {
    // Read guess
    read_status = getline(&line, &len, read_from_c);

    // perror and return
    if (read_status == -1) { perror("From Parent: Failed to read guess\n"); return 1; }
    
    // Parsing guess
    sscanf(line, "%d\n", &guess);
    printf("From Parent: Guess # %d, Child guessed %d\n", i + 1, guess);

    // Correct guess
    if (guess == secret) {
      printf("From Parent: Correct\n");
      fprintf(write_to_c, "Y\n");
      fflush(write_to_c);

      // Should not duplicate code
      // Break, and let outside of loop handle exit
      break;
    }

    // Reply bad guess
    printf("From Parent: Incorrect\n");
    fprintf(write_to_c, "N\n");
    fflush(write_to_c);
  }

  if (i == 100) {
    printf("From Parent: Ran out of guesses\n");
    fprintf(write_to_c, "E\n");
    fflush(write_to_c);
  }
  
  printf("From Parent: Cleaning up\n");
  
  int c_exit_status;
  wait(&c_exit_status);

  fclose(write_to_c);
  fclose(read_from_c);

  printf("From Parent: Exiting with Status:\n");
  printf("From Parent: Child: %d\n", WEXITSTATUS(c_exit_status));
  return 0;
}

int child_process(int p_to_c, int c_to_p) {

  // Manage pipes as 
  FILE* write_to_p = fdopen(c_to_p, "w");
  FILE* read_from_p = fdopen(p_to_c, "r");

  // Set up random seq
  srand(10);

  while (true) {
    int guess = rand() % 20;
    printf("From Child: Guessing %d\n", guess);

    fprintf(write_to_p, "%d\n", guess);
    fflush(write_to_p);

    // Prepare to read line
    char* line = NULL;
    size_t len;

    // Read result
    ssize_t read_status = getline(&line, &len, read_from_p);
    if (read_status == -1) { printf("From Child: Failed to read result\n"); exit(1); }

    // Parse result
    char guess_result = 'K';
    sscanf(line, "%c\n", &guess_result);
    printf("From Child: Guess result is %c\n", guess_result);

    // Correct guess
    if (guess_result == 'Y') { 
      printf("From Child: I guessed right\n");

      break;
    }
    // Out of guess
    else if (guess_result == 'E') { 
      printf("From Child: Ran out of guesses\n");

      break;
    }
    // Incorrect
    else if (guess_result == 'N') { 
      printf("From Child: I guessed wrong\n");
    }
  }

  fclose(write_to_p);
  fclose(read_from_p);
  
  printf("From Child: Exiting...\n");
  return 0;
  
}

int main() {
  int p_to_c[2]; // Parent to Child
  int c_to_p[2]; // Child to Parent

  int status = 0;

  // Create pipes
  status = pipe(p_to_c);
  // Use perror, don't use exit
  if (status == -1) { perror("Failed to create pipe\n"); return 1; } 
  status = pipe(c_to_p);
  // Use perror, don't use exit
  if (status == -1) { perror("Failed to create pipe\n"); return 1; }
  
  int fork_result = fork();

  if (fork_result == -1) {
    printf("Failed to fork\n");
    exit(1);
  }

  if (fork_result != 0) {
    // Close pipee before, only pass resourses needed
    close(p_to_c[0]);
    close(c_to_p[1]);
    
    return parent_process(p_to_c[1], c_to_p[0]);
  }

  if (fork_result == 0) {
    // Close pipes before, only pass resources needed
    close(p_to_c[1]);
    close(c_to_p[0]);
    
    return child_process(p_to_c[0], c_to_p[1]);
  }
    
  // Use return
  return 0;
}

