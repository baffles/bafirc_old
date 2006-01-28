#include "bafirc.h"
#include <time.h>

#define balloc malloc
#define bfree free
#define brealloc realloc
#define bcalloc calloc
#define print_mem_report(a)
#define print_mem_report_irc(a, b)

int main()
{
  int i;
  clock_t starta, enda, startd, endd;
  void *crap[5000];
  starta = clock();
  for(i = 0; i < 5000; ++i)
    crap[i] = balloc(1);
  enda = clock();
  startd = clock();
  for(i = 4999; i >= 0; --i)
    bfree(crap[i]);
  endd = clock();
  
  printf("balloc Allocation took %d clocks... Deallocation took %d clocks... Total time was %d clocks... Clocks per second: %d\n", enda - starta, endd - startd, (enda - starta) + (endd - startd), CLOCKS_PER_SEC);
  
  for(i = 0; i < 5000; ++i)
    crap[i] = malloc(1);
  enda = clock();
  startd = clock();
  for(i = 4999; i >= 0; --i)
    free(crap[i]);
  endd = clock();
  
  printf("malloc Allocation took %d clocks... Deallocation took %d clocks... Total time was %d clocks... Clocks per second: %d\n", enda - starta, endd - startd, (enda - starta) + (endd - startd), CLOCKS_PER_SEC);
}
