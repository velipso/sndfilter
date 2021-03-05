// (c) Copyright 2016, Sean Connelly (@velipso), https://sean.cm
// MIT License
// Project Home: https://github.com/velipso/sndfilter

#include "biquad.h"
#include <math.h>

// biquad filtering is based on a small sliding window, where the different filters are a result of
// simply changing the coefficients used while processing the samples
//
// the biquad filter processes a sound using 10 parameters:
//   b0, b1, b2, a1, a2      transformation coefficients
//   xn0, xn1, xn2           the unfiltered sample at position x[n], x[n-1], and x[n-2]
//   yn1, yn2                the filtered sample at position y[n-1] and y[n-2]
void sf_biquad_process(sf_biquad_state_st *state, int size, sf_sample_st *input,
	sf_sample_st *output){

	// pull out the state into local variables
	float b0 = state->b0;
	float b1 = state->b1;
	float b2 = state->b2;
	float a1 = state->a1;
	float a2 = state->a2;
	sf_sample_st xn1 = state->xn1;
	sf_sample_st xn2 = state->xn2;
	sf_sample_st yn1 = state->yn1;
	sf_sample_st yn2 = state->yn2;

	// loop for each sample
	for (int n = 0; n < size; n++){
		// get the current sample
		sf_sample_st xn0 = input[n];

		// the formula is the same for each channel
		float L =
			b0 * xn0.L +
			b1 * xn1.L +
			b2 * xn2.L -
			a1 * yn1.L -
			a2 * yn2.L;
		float R =
			b0 * xn0.R +
			b1 * xn1.R +
			b2 * xn2.R -
			a1 * yn1.R -
			a2 * yn2.R;

		// save the result
		output[n] = (sf_sample_st){ L, R };

		// slide everything down one sample
		xn2 = xn1;
		xn1 = xn0;
		yn2 = yn1;
		yn1 = output[n];
	}

	// save the state for future processing
	state->xn1 = xn1;
	state->xn2 = xn2;
	state->yn1 = yn1;
	state->yn2 = yn2;
}

// each type of filter just has some magic math to setup the coefficients
//
// the math is quite complicated to understand, but the *implementation* is quite simple
//
// I have no insight into the genius of the math -- you're on your own for that.  You might find
// some help in some of the articles here:
//   http://www.musicdsp.org/showmany.php
//
// formulas extracted and massaged from Chromium source, Biquad.cpp, here:
//   https://git.io/v10H2

// clear the samples saved across process boundaries
static inline void state_reset(sf_biquad_state_st *state){
	state->xn1 = (sf_sample_st){ 0, 0 };
	state->xn2 = (sf_sample_st){ 0, 0 };
	state->yn1 = (sf_sample_st){ 0, 0 };
	state->yn2 = (sf_sample_st){ 0, 0 };
}

// set the coefficients so that the output is the input scaled by `amt`
static inline void state_scale(sf_biquad_state_st *state, float amt){
	state->b0 = amt;
	state->b1 = 0.0f;
	state->b2 = 0.0f;
	state->a1 = 0.0f;
	state->a2 = 0.0f;
}

// set the coefficients so that the output is an exact copy of the input
static inline void state_passthrough(sf_biquad_state_st *state){
	state_scale(state, 1.0f);
}

// set the coefficients so that the output is zeroed out
static inline void state_zero(sf_biquad_state_st *state){
	state_scale(state, 0.0f);
}

// initialize the biquad state to be a lowpass filter
void sf_lowpass(sf_biquad_state_st *state, int rate, float cutoff, float resonance){
	state_reset(state);
	float nyquist = rate * 0.5f;
	cutoff /= nyquist;

	if (cutoff >= 1.0f)
		state_passthrough(state);
	else if (cutoff <= 0.0f)
		state_zero(state);
	else{
		resonance = powf(10.0f, resonance * 0.05f); // convert resonance from dB to linear
		float theta = (float)M_PI * 2.0f * cutoff;
		float alpha = sinf(theta) / (2.0f * resonance);
		float cosw  = cosf(theta);
		float beta  = (1.0f - cosw) * 0.5f;
		float a0inv = 1.0f / (1.0f + alpha);
		state->b0 = a0inv * beta;
		state->b1 = a0inv * 2.0f * beta;
		state->b2 = a0inv * beta;
		state->a1 = a0inv * -2.0f * cosw;
		state->a2 = a0inv * (1.0f - alpha);
	}
}

void sf_highpass(sf_biquad_state_st *state, int rate, float cutoff, float resonance){
	state_reset(state);
	float nyquist = rate * 0.5f;
	cutoff /= nyquist;

	if (cutoff >= 1.0f)
		state_zero(state);
	else if (cutoff <= 0.0f)
		state_passthrough(state);
	else{
		resonance = powf(10.0f, resonance * 0.05f); // convert resonance from dB to linear
		float theta = (float)M_PI * 2.0f * cutoff;
		float alpha = sinf(theta) / (2.0f * resonance);
		float cosw  = cosf(theta);
		float beta  = (1.0f + cosw) * 0.5f;
		float a0inv = 1.0f / (1.0f + alpha);
		state->b0 = a0inv * beta;
		state->b1 = a0inv * -2.0f * beta;
		state->b2 = a0inv * beta;
		state->a1 = a0inv * -2.0f * cosw;
		state->a2 = a0inv * (1.0f - alpha);
	}
}

void sf_bandpass(sf_biquad_state_st *state, int rate, float freq, float Q){
	state_reset(state);
	float nyquist = rate * 0.5f;
	freq /= nyquist;

	if (freq <= 0.0f || freq >= 1.0f)
		state_zero(state);
	else if (Q <= 0.0f)
		state_passthrough(state);
	else{
		float w0    = (float)M_PI * 2.0f * freq;
		float alpha = sinf(w0) / (2.0f * Q);
		float k     = cosf(w0);
		float a0inv = 1.0f / (1.0f + alpha);
		state->b0 = a0inv * alpha;
		state->b1 = 0;
		state->b2 = a0inv * -alpha;
		state->a1 = a0inv * -2.0f * k;
		state->a2 = a0inv * (1.0f - alpha);
	}
}

void sf_notch(sf_biquad_state_st *state, int rate, float freq, float Q){
	state_reset(state);
	float nyquist = rate * 0.5f;
	freq /= nyquist;

	if (freq <= 0.0f || freq >= 1.0f)
		state_passthrough(state);
	else if (Q <= 0.0f)
		state_zero(state);
	else{
		float w0    = (float)M_PI * 2.0f * freq;
		float alpha = sinf(w0) / (2.0f * Q);
		float k     = cosf(w0);
		float a0inv = 1.0f / (1.0f + alpha);
		state->b0 = a0inv;
		state->b1 = a0inv * -2.0f * k;
		state->b2 = a0inv;
		state->a1 = a0inv * -2.0f * k;
		state->a2 = a0inv * (1.0f - alpha);
	}
}

void sf_peaking(sf_biquad_state_st *state, int rate, float freq, float Q, float gain){
	state_reset(state);
	float nyquist = rate * 0.5f;
	freq /= nyquist;

	if (freq <= 0.0f || freq >= 1.0f){
		state_passthrough(state);
		return;
	}

	float A = powf(10.0f, gain * 0.025f); // square root of gain converted from dB to linear

	if (Q <= 0.0f){
		state_scale(state, A * A); // scale by A squared
		return;
	}

	float w0    = (float)M_PI * 2.0f * freq;
	float alpha = sinf(w0) / (2.0f * Q);
	float k     = cosf(w0);
	float a0inv = 1.0f / (1.0f + alpha / A);
	state->b0 = a0inv * (1.0f + alpha * A);
	state->b1 = a0inv * -2.0f * k;
	state->b2 = a0inv * (1.0f - alpha * A);
	state->a1 = a0inv * -2.0f * k;
	state->a2 = a0inv * (1.0f - alpha / A);
}

void sf_allpass(sf_biquad_state_st *state, int rate, float freq, float Q){
	state_reset(state);
	float nyquist = rate * 0.5f;
	freq /= nyquist;

	if (freq <= 0.0f || freq >= 1.0f)
		state_passthrough(state);
	else if (Q <= 0.0f)
		state_scale(state, -1.0f); // invert the sample
	else{
		float w0    = (float)M_PI * 2.0f * freq;
		float alpha = sinf(w0) / (2.0f * Q);
		float k     = cosf(w0);
		float a0inv = 1.0f / (1.0f + alpha);
		state->b0 = a0inv * (1.0f - alpha);
		state->b1 = a0inv * -2.0f * k;
		state->b2 = a0inv * (1.0f + alpha);
		state->a1 = a0inv * -2.0f * k;
		state->a2 = a0inv * (1.0f - alpha);
	}
}

// WebAudio hardcodes Q=1
void sf_lowshelf(sf_biquad_state_st *state, int rate, float freq, float Q, float gain){
	state_reset(state);
	float nyquist = rate * 0.5f;
	freq /= nyquist;

	if (freq <= 0.0f || Q == 0.0f){
		state_passthrough(state);
		return;
	}

	float A = powf(10.0f, gain * 0.025f); // square root of gain converted from dB to linear

	if (freq >= 1.0f){
		state_scale(state, A * A); // scale by A squared
		return;
	}

	float w0    = (float)M_PI * 2.0f * freq;
	float ainn  = (A + 1.0f / A) * (1.0f / Q - 1.0f) + 2.0f;
	if (ainn < 0)
		ainn = 0;
	float alpha = 0.5f * sinf(w0) * sqrtf(ainn);
	float k     = cosf(w0);
	float k2    = 2.0f * sqrtf(A) * alpha;
	float Ap1   = A + 1.0f;
	float Am1   = A - 1.0f;
	float a0inv = 1.0f / (Ap1 + Am1 * k + k2);
	state->b0 = a0inv * A * (Ap1 - Am1 * k + k2);
	state->b1 = a0inv * 2.0f * A * (Am1 - Ap1 * k);
	state->b2 = a0inv * A * (Ap1 - Am1 * k - k2);
	state->a1 = a0inv * -2.0f * (Am1 + Ap1 * k);
	state->a2 = a0inv * (Ap1 + Am1 * k - k2);
}

// WebAudio hardcodes Q=1
void sf_highshelf(sf_biquad_state_st *state, int rate, float freq, float Q, float gain){
	state_reset(state);
	float nyquist = rate * 0.5f;
	freq /= nyquist;

	if (freq >= 1.0f || Q == 0.0f){
		state_passthrough(state);
		return;
	}

	float A = powf(10.0f, gain * 0.025f); // square root of gain converted from dB to linear

	if (freq <= 0.0f){
		state_scale(state, A * A); // scale by A squared
		return;
	}

	float w0    = (float)M_PI * 2.0f * freq;
	float ainn  = (A + 1.0f / A) * (1.0f / Q - 1.0f) + 2.0f;
	if (ainn < 0)
		ainn = 0;
	float alpha = 0.5f * sinf(w0) * sqrtf(ainn);
	float k     = cosf(w0);
	float k2    = 2.0f * sqrtf(A) * alpha;
	float Ap1   = A + 1.0f;
	float Am1   = A - 1.0f;
	float a0inv = 1.0f / (Ap1 - Am1 * k + k2);
	state->b0 = a0inv * A * (Ap1 + Am1 * k + k2);
	state->b1 = a0inv * -2.0f * A * (Am1 + Ap1 * k);
	state->b2 = a0inv * A * (Ap1 + Am1 * k - k2);
	state->a1 = a0inv * 2.0f * (Am1 - Ap1 * k);
	state->a2 = a0inv * (Ap1 - Am1 * k - k2);
}
