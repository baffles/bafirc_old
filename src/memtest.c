#include "bafirc/mem.h"

int main()
{
  void *m1, *m2, *m3;
  printf("balloc demo\n");
  print_mem_report(stdout);
  printf("Allocating some memory!\n");
  m1 = balloc(1024);
  printf("Allocated 1024 bytes\n");
  print_mem_report(stdout);
  printf("Allocating some memory!\n");
  m2 = balloc(2048);
  printf("Allocated 2048 bytes\n");
  print_mem_report(stdout);
  printf("Freeing some memory!\n");
  bfree(m1);
  printf("Freed 1024 bytes\n");
  print_mem_report(stdout);
  printf("Allocating some memory!\n");
  m1 = balloc(4096);
  m3 = balloc(10240);
  printf("Allocated 14336 bytes\n");
  print_mem_report(stdout);
  printf("Freeing it all!\n");
  bfree(m1);
  bfree(m2);
  bfree(m3);
  print_mem_report(stdout);
  printf("Done! Have fun!\n");
}
