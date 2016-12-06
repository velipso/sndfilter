// (c) Copyright 2016, Sean Connelly (@voidqk), http://syntheti.cc
// MIT License
// Project Home: https://github.com/voidqk/sndfilter

#ifndef SNDFILTER_TYPES__H
#define SNDFILTER_TYPES__H

#include <stdlib.h>

typedef struct {
	float L; // left channel sample
	float R; // right channel sample
} sf_sample_st;

typedef struct {
	sf_sample_st *samples;
	int size; // number of samples
	int rate; // samples per second
} sf_snd_st, *sf_snd;

// simple sf_snd constructor
static inline sf_snd sf_snd_new(){
	sf_snd s = malloc(sizeof(sf_snd_st));
	if (s == NULL)
		return NULL;
	s->samples = NULL;
	s->size = 0;
	s->rate = 0;
	return s;
}

// simple sf_snd destructor
static inline void sf_snd_free(sf_snd snd){
	if (snd->samples)
		free(snd->samples);
	free(snd);
}

#endif // SNDFILTER_TYPES__H
