// (c) Copyright 2016, Sean Connelly (@velipso), https://sean.cm
// MIT License
// Project Home: https://github.com/velipso/sndfilter

// simple .wav file loading and saving
// only handles loading 1 or 2 channel WAVs with 16-bit samples
// only saves 2 channel WAVs with 16-bit samples

#ifndef SNDFILTER_WAV__H
#define SNDFILTER_WAV__H

#include "sf_common.h"
#include "snd.h"

SF_API_BEGIN

sf_snd sf_wavload(const char *file);
bool   sf_wavsave(sf_snd snd, const char *file);

SF_API_END

#endif // SNDFILTER_WAV__H
