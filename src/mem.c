//
// sndfilter - Algorithms for sound filters, like reverb, lowpass, etc
// by Sean Connelly (@velipso), https://sean.fun
// Project Home: https://github.com/velipso/sndfilter
// SPDX-License-Identifier: 0BSD
//

#include "mem.h"
#include <stdlib.h>

// initialize sf_malloc/sf_free with the standard malloc/free
sf_malloc_func sf_malloc = malloc;
sf_free_func sf_free = free;
