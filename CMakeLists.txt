cmake_minimum_required(VERSION 3.7)

#include(src)
project(cmake C)

file(GLOB SF_SRC
  src/mem.c
  src/snd.c
  src/wav.c
  src/biquad.c
  src/compressor.c
  src/reverb.c
)
add_library(sndfilter STATIC ${SF_SRC})
