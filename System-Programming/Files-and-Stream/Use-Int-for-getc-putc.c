#include <stdio.h>

int main(void)
{
  FILE *f;
  int i;
  int intvar;
  char charvar;

  /* 1. Write three bytes to testfile, they're all 255. */
  if ((f = fopen("testfile", "w")) == NULL) {
    fprintf(stderr, "Can't open testfile for writing.");
    return 1;
  }
  for (i = 0; i < 3; i++) {
    putc(255, f);
  }
  fclose(f);

  /* The significance of 255:
   * * It is -1 in char (signed 1 byte on this system).
   * * EOF is also -1 (but int) on this system.
   * So they are indistinguishable if you store in char.
   */

  /* 2. Read and count testfile, but wrong: using charvar. */
  if ((f = fopen("testfile", "r")) == NULL) {
    fprintf(stderr, "Can't open testfile.");
    return 1;
  }
  for (i = 0; (charvar = getc(f)) != EOF; i++) {
  }
  fclose(f);
  printf("Wrong count = %d\n", i);

  /* 3. Read and count testfile, but right: using intvar. */
  if ((f = fopen("testfile", "r")) == NULL) {
    fprintf(stderr, "Can't open testfile.");
    return 1;
  }
  for (i = 0; (intvar = getc(f)) != EOF; i++) {
  }
  fclose(f);
  printf("Right count = %d\n", i);

  putchar(97);
  putchar(127);
  putchar(128);
  putchar(129);
  putchar(255);
  
  return 0;
}
