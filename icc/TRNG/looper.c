/*************************************************************************
// Copyright IBM Corp. 2023
//
// Licensed under the Apache License 2.0 (the "License"). You may not use
// this file except in compliance with the License. You can obtain a copy
// in the file LICENSE in the source distribution.
*************************************************************************/


#include <string.h>
/*! @brief delay loop
    It's in this module exactly because it's not used by any code in here
    so the optimizer can't see it and make it a NOOP and hopefully the optimizer in
    other modules can't see it either
    @param i pointer to the loop iterator
    @param j pointer to the loop count
    @return total, just so that this appears to do something
*/

#if defined(_WIN32)
#  pragma optimize("",off)
#endif

#if defined(__linux)
#  pragma GCC push_options
#  pragma GCC optimize ("O0")
#endif

#  if !defined(MEM_TIMING)
int looper(volatile int *i,volatile int *j)
{
  int k = 0;
  for( ; (*i) < (*j); (*i)++ ) k++;
  return k;
}
#  else
/* Use memory access instead, loop over a memory region. i is unused in this case  */

volatile unsigned char mybuffer[1024];

int looper(volatile int *i,volatile int *j)
{
  int k = 0;
  volatile  int i1 = 0;
  int l =sizeof(mybuffer)-16;
  for (i1 = 0; i1 < *j; i1+=16) {
    if(i1 >= l) {
      i1 = 0;
    }
    memmove((void *)mybuffer+i1,(void *)(mybuffer+i1+16),16);
    k += (mybuffer[i1]);
  }
  return k;
}
#  endif

#if defined(__linux)
#  pragma GCC pop_options
#endif

#if defined(_WIN32)
#  pragma optimize("",on)
#endif
