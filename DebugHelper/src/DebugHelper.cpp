#include "DebugHelper.h"

#ifdef DO_RTT
RTT_Helper rtt;
#endif

extern "C" char *sbrk(int i);

int32_t FreeRam() {
  char stack_dummy = 0;
  return &stack_dummy - sbrk(0);
}
