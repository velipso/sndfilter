// (c) Copyright 2016, Sean Connelly (@velipso), https://sean.cm
// MIT License
// Project Home: https://github.com/velipso/sndfilter

// memory management

#ifndef SNDFILTER_MEM__H
#define SNDFILTER_MEM__H

#include <stddef.h>

typedef void *(*sf_malloc_func)(size_t size);
typedef void (*sf_free_func)(void *ptr);

// overwrite these globals to change the malloc/free functions of the library
extern sf_malloc_func sf_malloc;
extern sf_free_func sf_free;

#endif // SNDFILTER_MEM__H
