// (c) Copyright 2016, Sean Connelly (@voidqk), http://syntheti.cc
// MIT License
// Project Home: https://github.com/voidqk/sndfilter

// simple .wav file loading and saving

#ifndef SNDFILTER_WAV__H
#define SNDFILTER_WAV__H

#include "types.h"
#include <stdbool.h>

sf_snd sf_wavload(const char *file);
bool   sf_wavsave(sf_snd snd, const char *file);

#endif // SNDFILTER_WAV__H
