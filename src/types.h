// (c) Copyright 2016, Sean Connelly (@voidqk), http://syntheti.cc
// MIT License
// Project Home: https://github.com/voidqk/sndfilter

#ifndef SNDFILTER_TYPES__H
#define SNDFILTER_TYPES__H

typedef struct {
	float L; // left channel sample
	float R; // right channel sample
} sf_sample_st;

typedef struct {
	sf_sample_st *samples;
	int size; // number of samples
	int rate; // samples per second
} sf_snd_st, *sf_snd;

#endif // SNDFILTER_TYPES__H
