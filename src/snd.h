// (c) Copyright 2016, Sean Connelly (@voidqk), http://syntheti.cc
// MIT License
// Project Home: https://github.com/voidqk/sndfilter

// data structure for a 2-channel 32-bit floating point sound in memory

#ifndef SNDFILTER_SND__H
#define SNDFILTER_SND__H

#include <stdbool.h>

typedef struct {
	float L; // left channel sample
	float R; // right channel sample
} sf_sample_st;

typedef struct {
	sf_sample_st *samples;
	int size; // number of samples
	int rate; // samples per second
} sf_snd_st, *sf_snd;

sf_snd sf_snd_new(int size, int rate, bool clear);
void   sf_snd_free(sf_snd snd);

#endif // SNDFILTER_SND__H
