#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "mvp.h"

//you should compile with -Wall -Werror and include makefiles
int main() {
  int p_to_c[2]; // Parent to Child
  int c_to_p[2]; // Child to Parent

  //Good!
  int status = 0;

  // Create pipes
  status = pipe(p_to_c);
  //mistake: using printf instead of perror.
  if (status == -1) { printf("Failed to create pipe\n"); exit(1); }
  status = pipe(c_to_p);
  //mistake: exit(1) instead of return
  //mistake: using printf instead of perror.
  if (status == -1) { printf("Failed to create pipe\n"); exit(1); }
  
  //BUG: pid_t not int.
  int fork_result = fork();

  if (fork_result == -1) {
    //perror
    printf("Failed to fork\n");
    //mistake: exit 1
    exit(1);
  }

  if (fork_result != 0) { // Parents process
    //don't put comments like ^^^^^^^^^^^^^^^
    //good use of functions
    parent_process(p_to_c, c_to_p);
  }

  if (fork_result == 0) { // Child process
    child_process(p_to_c, c_to_p);
  }
    
  //return 0 instead of exit(0).
  exit(0);
}

//int pointer is wrong.
//just pass the file descriptor by value.
int parent_process(int* p_to_c, int* c_to_p) {
  // Manage pipes
  // close the pipes before calling parent process. You should only pass resources to a function that that function will acutally use.
  close(p_to_c[0]);
  close(c_to_p[1]);

  // Manage pipes as FILE/STREAMs
  // "a" is not needed, just "w" is fine.
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

    
    //don't declare the guess variable inside the loop, it wastes space.
    // Read guess
    int guess;
    fscanf(read_from_c, "%d", &guess);
    printf("From Parent: Guess # %d, Child guessed %d\n", i + 1, guess);

    // Correct guess
    if (guess == secret) {
      printf("From Parent: Correct\n");
      //I notice you use fprintf -> fflush a lot of times. You should either make a helper function to avoid the code duplication, 
      fprintf(write_to_c, "Y");
      fflush(write_to_c);

      //calling subfunctions that call exit() is really bad practice. You should almost always only exit using return in the main function. 
      //this is because you might have other code that needs to clean up, which can't happen if exit() is called deeply nested.
      //in general, exit() is always the wrong choice.
      wait_and_exit(write_to_c, read_from_c);
    }

    // puts instead of printf
    printf("From Parent: Incorrect\n");
    fprintf(write_to_c, "N");
    fflush(write_to_c);
  }

  //puts
  printf("From Parent: Ran out of guesses\n");
  fprintf(write_to_c, "E");
  fflush(write_to_c);

  //you have duplicated calls to wait_and_exit with the same parameters, even though you only need to call this function once.
  wait_and_exit(write_to_c, read_from_c);

}

int child_process(int* p_to_c, int* c_to_p) {
  // Manage pipes
  // don't pass arguments you don't use.
  close(p_to_c[1]);
  close(c_to_p[0]);

  // Manage pipes as 
  // imo I'd prefer this conversion was done in the parent.
  FILE* write_to_p = fdopen(c_to_p[1], "a");
  FILE* read_from_p = fdopen(p_to_c[0], "r");

  // Set up random seq
  srand(10);

  while (true) {
    int guess = rand() % 20;
    printf("From Child: Guessing %d\n", guess);
    char guess_result = 'K';

    //fprintf -> fflush
    fprintf(write_to_p, "%d\n", guess);
    fflush(write_to_p);

    fscanf(read_from_p, "%c", &guess_result);
    printf("From Child: Guess result is %c\n", guess_result);

    if (guess_result == 'Y') { // Correct
                               // ^^^pls no :(
      printf("From Child: I guessed right\n");
      //inconsistent naming, you shouldn't use a helper function for this.
      close_child_STREAMs(write_to_p, read_from_p);

      //subfunctions that call exit 
      exit_child();
      //inconsistent use of } with else if
    }
    else if (guess_result == 'E') { // Out of guess
      printf("From Child: Ran out of guesses\n");
      //calling same subfunction on same arguments => not needed code duplication.
      close_child_STREAMs(write_to_p, read_from_p);
      //duplicated call to exit_child.
      exit_child();
    }
    else if (guess_result == 'N') { // Incorrect
      //puts
      printf("From Child: I guessed wrong\n");
    }
  }
  
}

//typically you want to write helper functions ABOVE the function that calls them instead of below, that way you don't need a header.
//also all these functions should be static, since you don't want to export them to other files.
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
