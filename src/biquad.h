// (c) Copyright 2016, Sean Connelly (@velipso), https://sean.cm
// MIT License
// Project Home: https://github.com/velipso/sndfilter

// biquad filtering based on WebAudio specification:
//   https://webaudio.github.io/web-audio-api/#the-biquadfilternode-interface

#ifndef SNDFILTER_BIQUAD__H
#define SNDFILTER_BIQUAD__H

#include "snd.h"

// biquad filtering is a technique used to perform a variety of sound filters
//
// this API works by first initializing an sf_biquad_state_st structure, and then using it to
// process a sample in chunks
//
// for example, for a lowpass filter over a stream with 128 samples per chunk, you would do:
//
//   sf_biquad_state_st lowpass;
//   sf_lowpass(&lowpass, 44100, 440, 1);
//
//   for each 128 length sample:
//     sf_biquad_process(&lowpass, 128, input, output);
//
// notice that sf_biquad_process will change the xn1,xn2,yn1,yn2 values inside of the state
// structure, since these values must be carried over across chunk boundaries
//
// also notice that the choice to divide the sound into chunks of 128 samples is completely
// arbitrary from the filter's perspective

typedef struct {
	float b0;
	float b1;
	float b2;
	float a1;
	float a2;
	sf_sample_st xn1;
	sf_sample_st xn2;
	sf_sample_st yn1;
	sf_sample_st yn2;
} sf_biquad_state_st;

// these functions will initialize an sf_biquad_state_st structure based on the desired filter
void sf_lowpass  (sf_biquad_state_st *state, int rate, float cutoff, float resonance);
void sf_highpass (sf_biquad_state_st *state, int rate, float cutoff, float resonance);
void sf_bandpass (sf_biquad_state_st *state, int rate, float freq, float Q);
void sf_notch    (sf_biquad_state_st *state, int rate, float freq, float Q);
void sf_peaking  (sf_biquad_state_st *state, int rate, float freq, float Q, float gain);
void sf_allpass  (sf_biquad_state_st *state, int rate, float freq, float Q);
void sf_lowshelf (sf_biquad_state_st *state, int rate, float freq, float Q, float gain);
void sf_highshelf(sf_biquad_state_st *state, int rate, float freq, float Q, float gain);

// this function will process the input sound based on the state passed
// the input and output buffers should be the same size
void sf_biquad_process(sf_biquad_state_st *state, int size, sf_sample_st *input,
	sf_sample_st *output);

#endif // SNDFILTER_BIQUAD__H
