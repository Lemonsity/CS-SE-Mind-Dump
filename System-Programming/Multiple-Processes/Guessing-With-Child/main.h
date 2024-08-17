#ifndef SELF_GUESS
#define SELF_GUESS

int parent_process(int* p_to_c, int* c_to_p);
int child_process(int* p_to_c, int* c_to_p);
int close_parent_STREAMs(FILE* write_to_c, FILE* read_from_c);
int close_child_STREAMs(FILE* write_to_p, FILE* read_from_p);
int exit_child();
int wait_and_exit(FILE* write_to_c, FILE* read_from_c);

#endif
