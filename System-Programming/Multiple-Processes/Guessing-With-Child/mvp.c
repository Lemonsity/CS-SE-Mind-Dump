#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "mvp.h"

int main() {
  int p_to_c[2]; // Parent to Child
  int c_to_p[2]; // Child to Parent

  int status = 0;

  // Create pipes
  status = pipe(p_to_c);
  if (status == -1) { printf("Failed to create pipe\n"); exit(1); }
  status = pipe(c_to_p);
  if (status == -1) { printf("Failed to create pipe\n"); exit(1); }
  
  int fork_result = fork();

  if (fork_result == -1) {
    printf("Failed to fork\n");
    exit(1);
  }

  if (fork_result != 0) { // Parents process
    parent_process(p_to_c, c_to_p);
  }

  if (fork_result == 0) { // Child process
    child_process(p_to_c, c_to_p);
  }
    
  exit(0);
}

int parent_process(int* p_to_c, int* c_to_p) {
  // Manage pipes
  close(p_to_c[0]);
  close(c_to_p[1]);

  // Manage pipes as FILE/STREAMs
  FILE* write_to_c = fdopen(p_to_c[1], "a");
  FILE* read_from_c = fdopen(c_to_p[0], "r");
  
  // Set up random seq
  srand(0);
  int secret = rand() % 20;
  // Uncomment the following line to see what happens when run out of guesses
  // secret = 100;
  printf("From Parent: Secret is %d\n", secret);
  
  // Limit to 100 guesses
  for (int i = 0; i < 100; i++) {

    
    // Read guess
    int guess;
    fscanf(read_from_c, "%d", &guess);
    printf("From Parent: Guess # %d, Child guessed %d\n", i + 1, guess);

    // Correct guess
    if (guess == secret) {
      printf("From Parent: Correct\n");
      fprintf(write_to_c, "Y");
      fflush(write_to_c);

      wait_and_exit(write_to_c, read_from_c);
    }

    // Reply bad guess
    printf("From Parent: Incorrect\n");
    fprintf(write_to_c, "N");
    fflush(write_to_c);
  }

  printf("From Parent: Ran out of guesses\n");
  fprintf(write_to_c, "E");
  fflush(write_to_c);

  wait_and_exit(write_to_c, read_from_c);

}

int child_process(int* p_to_c, int* c_to_p) {
  // Manage pipes
  close(p_to_c[1]);
  close(c_to_p[0]);

  // Manage pipes as 
  FILE* write_to_p = fdopen(c_to_p[1], "a");
  FILE* read_from_p = fdopen(p_to_c[0], "r");

  // Set up random seq
  srand(10);

  while (true) {
    int guess = rand() % 20;
    printf("From Child: Guessing %d\n", guess);
    char guess_result = 'K';

    fprintf(write_to_p, "%d\n", guess);
    fflush(write_to_p);

    fscanf(read_from_p, "%c", &guess_result);
    printf("From Child: Guess result is %c\n", guess_result);

    if (guess_result == 'Y') { // Correct
      printf("From Child: I guessed right\n");
      close_child_STREAMs(write_to_p, read_from_p);
      exit_child();
    }
    else if (guess_result == 'E') { // Out of guess
      printf("From Child: Ran out of guesses\n");
      close_child_STREAMs(write_to_p, read_from_p);
      exit_child();
    }
    else if (guess_result == 'N') { // Incorrect
      printf("From Child: I guessed wrong\n");
    }
  }
  
}

int close_parent_STREAMs(FILE* write_to_c, FILE* read_from_c) {
  fclose(write_to_c);
  fclose(read_from_c);
}

int close_child_STREAMs(FILE* write_to_p, FILE* read_from_p) {
  fclose(write_to_p);
  fclose(read_from_p);
}

int exit_child() {
  printf("From Child: Exiting...\n");
  exit(0);
}

int wait_and_exit(FILE* write_to_c, FILE* read_from_c) {
  printf("From Parent: Cleaning up\n");
  
  int c_exit_status;
  wait(&c_exit_status);

  close_parent_STREAMs(write_to_c, read_from_c);

  printf("From Parent: Exiting with Status:\n");
  printf("From Parent: Child: %d\n", WEXITSTATUS(c_exit_status));
  exit(0);
}
