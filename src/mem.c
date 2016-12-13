// (c) Copyright 2016, Sean Connelly (@voidqk), http://syntheti.cc
// MIT License
// Project Home: https://github.com/voidqk/sndfilter

#include "mem.h"
#include <stdlib.h>

// initialize sf_malloc/sf_free with the standard malloc/free
sf_malloc_func sf_malloc = malloc;
sf_free_func sf_free = free;
