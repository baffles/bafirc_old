#include "bafirc.h"
#include <time.h>

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
  
  printf("Allocation took %d clocks... Deallocation took %d clocks... Total time was %d clocks... Clocks per second: %d\n", enda - starta, endd - startd, (enda - starta) + (endd - startd), CLOCKS_PER_SEC);
}
