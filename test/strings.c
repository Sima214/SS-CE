#include <stdio.h>
#include <stdint.h>

#include <string/Strings.h>
#include <clock/Clock.h>

int main(int argc, char* argv[]) {
  char t1[] = {0 , 1 , 2 , 3 , 4 , 5 , 6 , 7 , 8 , 9 , 10, 11, 12, 13, 14, 15};
  char t2[] = {16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31};
  PerfClock pc;
  ssce_reset(&pc);
  ssce_start(&pc);
  for(int i=0; i<1024*1024*512; i++) {
    memswap_avx(t1, t2, 13);
  }
  ssce_stop(&pc);
  printf("%f\n", pc.delta);
  return 0;
}