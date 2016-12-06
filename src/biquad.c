// (c) Copyright 2016, Sean Connelly (@voidqk), http://syntheti.cc
// MIT License
// Project Home: https://github.com/voidqk/sndfilter

#include "biquad.h"
#include <math.h>

// this function is the workhorse that performs the magic
// the different types of filters simply change the coefficient parameters
static inline sf_snd biquad_filter(sf_snd x, float b0, float b1, float b2, float a1, float a2){
	// the biquad filter processes a sound using 10 parameters:
	//   b0, b1, b2, a1, a2      transformation coefficients
	//   xn0, xn1, xn2           the unfiltered sample at position x[n], x[n-1], and x[n-2]
	//   yn1, yn2                the filtered sample at position y[n-1] and y[n-2]

	// allocate a new sound
	sf_snd y = sf_snd_new();
	if (y == NULL)
		return NULL;
	y->size = x->size;
	y->rate = x->rate;
	y->samples = malloc(sizeof(sf_sample_st) * y->size);
	if (y->samples == NULL){
		sf_snd_free(y);
		return NULL;
	}

	// initialize xn1, xn2, yn1, yn2 to 0
	// note: a streaming sample would be processed in chunks, so these values would need to persist
	//       across chunk boundaries -- this algorithm process an entire sample at once, so that
	//       isn't needed here
	sf_sample_st xn1 = { 0, 0 };
	sf_sample_st xn2 = { 0, 0 };
	sf_sample_st yn1 = { 0, 0 };
	sf_sample_st yn2 = { 0, 0 };

	// loop for each sample
	for (int n = 0; n < x->size; n++){
		// get the current sample
		sf_sample_st xn0 = x->samples[n];

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
		y->samples[n] = (sf_sample_st){ L, R };

		// slide everything down one sample
		xn2 = xn1;
		xn1 = xn0;
		yn2 = yn1;
		yn1 = y->samples[n];
	}
	return y;
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

sf_snd sf_lowpass(sf_snd snd, float cutoff, float resonance){
	float nyquist = snd->rate * 0.5f;
	cutoff /= nyquist;

	if (cutoff >= 1.0f)
		return biquad_filter(snd, 1, 0, 0, 0, 0); // copy sample directly
	else if (cutoff <= 0.0f)
		return biquad_filter(snd, 0, 0, 0, 0, 0); // zero everything out

	resonance = powf(10.0f, resonance * 0.05f);
	float theta = (float)M_PI * 2.0f * cutoff;
	float alpha = sinf(theta) / (2.0f * resonance);
	float cosw  = cosf(theta);
	float beta  = (1.0f - cosw) * 0.5f;
	float a0inv = 1.0f / (1.0f + alpha);
	return biquad_filter(snd,
		a0inv * beta,          // b0
		a0inv * 2.0f * beta,   // b1
		a0inv * beta,          // b2
		a0inv * -2.0f * cosw,  // a1
		a0inv * (1.0f - alpha) // a2
	);
}

sf_snd sf_highpass(sf_snd snd, float cutoff, float resonance){
	float nyquist = snd->rate * 0.5f;
	cutoff /= nyquist;

	if (cutoff >= 1.0f)
		return biquad_filter(snd, 0, 0, 0, 0, 0); // zero everything out
	else if (cutoff <= 0.0f)
		return biquad_filter(snd, 1, 0, 0, 0, 0); // copy sample directly

	resonance = powf(10.0f, resonance * 0.05f);
	float theta = (float)M_PI * 2.0f * cutoff;
	float alpha = sinf(theta) / (2.0f * resonance);
	float cosw  = cosf(theta);
	float beta  = (1.0f + cosw) * 0.5f;
	float a0inv = 1.0f / (1.0f + alpha);
	return biquad_filter(snd,
		a0inv * beta,          // b0
		a0inv * -2.0f * beta,  // b1
		a0inv * beta,          // b2
		a0inv * -2.0f * cosw,  // a1
		a0inv * (1.0f - alpha) // a2
	);
}

sf_snd sf_bandpass(sf_snd snd, float freq, float Q){
	float nyquist = snd->rate * 0.5f;
	freq /= nyquist;

	if (freq <= 0.0f || freq >= 1.0f)
		return biquad_filter(snd, 0, 0, 0, 0, 0); // zero everything out
	else if (Q <= 0.0f)
		return biquad_filter(snd, 1, 0, 0, 0, 0); // copy sample directly

	float w0    = (float)M_PI * 2.0f * freq;
	float alpha = sinf(w0) / (2.0f * Q);
	float k     = cosf(w0);
	float a0inv = 1.0f / (1.0f + alpha);
	return biquad_filter(snd,
		a0inv * alpha,         // b0
		0,                     // b1
		a0inv * -alpha,        // b2
		a0inv * -2.0f * k,     // a1
		a0inv * (1.0f - alpha) // a2
	);
}

sf_snd sf_notch(sf_snd snd, float freq, float Q){
	float nyquist = snd->rate * 0.5f;
	freq /= nyquist;

	if (freq <= 0.0f || freq >= 1.0f)
		return biquad_filter(snd, 1, 0, 0, 0, 0); // copy sample directly
	else if (Q <= 0.0f)
		return biquad_filter(snd, 0, 0, 0, 0, 0); // zero everything out

	float w0    = (float)M_PI * 2.0f * freq;
	float alpha = sinf(w0) / (2.0f * Q);
	float k     = cosf(w0);
	float a0inv = 1.0f / (1.0f + alpha);
	return biquad_filter(snd,
		a0inv,                 // b0
		a0inv * -2.0f * k,     // b1
		a0inv,                 // b2
		a0inv * -2.0f * k,     // a1
		a0inv * (1.0f - alpha) // a2
	);
}

sf_snd sf_peaking(sf_snd snd, float freq, float Q, float gain){
	float nyquist = snd->rate * 0.5f;
	freq /= nyquist;

	if (freq <= 0.0f || freq >= 1.0f)
		return biquad_filter(snd, 1, 0, 0, 0, 0); // copy sample directly

	float A = powf(10.0f, gain * 0.025f);

	if (Q <= 0.0f)
		return biquad_filter(snd, A * A, 0, 0, 0, 0); // scale by A squared

	float w0    = (float)M_PI * 2.0f * freq;
	float alpha = sinf(w0) / (2.0f * Q);
	float k     = cosf(w0);
	float a0inv = 1.0f / (1.0f + alpha / A);
	return biquad_filter(snd,
		a0inv * (1.0f + alpha * A), // b0
		a0inv * -2.0f * k,          // b1
		a0inv * (1.0f - alpha * A), // b2
		a0inv * -2.0f * k,          // a1
		a0inv * (1.0f - alpha / A)  // a2
	);
}

sf_snd sf_allpass(sf_snd snd, float freq, float Q){
	float nyquist = snd->rate * 0.5f;
	freq /= nyquist;

	if (freq <= 0.0f || freq >= 1.0f)
		return biquad_filter(snd, 1, 0, 0, 0, 0); // copy sample directly
	else if (Q <= 0.0f)
		return biquad_filter(snd, -1, 0, 0, 0, 0); // invert the sample

	float w0    = (float)M_PI * 2.0f * freq;
	float alpha = sinf(w0) / (2.0f * Q);
	float k     = cosf(w0);
	float a0inv = 1.0f / (1.0f + alpha);
	return biquad_filter(snd,
		a0inv * (1.0f - alpha), // b0
		a0inv * -2.0f * k,      // b1
		a0inv * (1.0f + alpha), // b2
		a0inv * -2.0f * k,      // a1
		a0inv * (1.0f - alpha)  // a2
	);
}

sf_snd sf_lowshelf(sf_snd snd, float freq, float Q, float gain){ // WebAudio hardcodes Q=1
	float nyquist = snd->rate * 0.5f;
	freq /= nyquist;

	// TODO: figure out what happens when Q <= 0 or Q >= 1

	if (freq <= 0.0f)
		return biquad_filter(snd, 1, 0, 0, 0, 0); // copy sample directly

	float A = powf(10.0f, gain * 0.025f);

	if (freq >= 1.0f)
		return biquad_filter(snd, A * A, 0, 0, 0, 0); // scale by A squared

	float w0    = (float)M_PI * 2.0f * freq;
	float alpha = 0.5f * sinf(w0) * sqrtf((A + 1.0f / A) * (1.0f / Q - 1.0f) + 2.0f);
	float k     = cosf(w0);
	float k2    = 2.0f * sqrtf(A) * alpha;
	float Ap1   = A + 1.0f;
	float Am1   = A - 1.0f;
	float a0inv = 1.0f / (Ap1 + Am1 * k + k2);
	return biquad_filter(snd,
		a0inv * A * (Ap1 - Am1 * k + k2),   // b0
		a0inv * 2.0f * A * (Am1 - Ap1 * k), // b1
		a0inv * A * (Ap1 - Am1 * k - k2),   // b2
		a0inv * -2.0f * (Am1 + Ap1 * k),    // a1
		a0inv * (Ap1 + Am1 * k - k2)        // a2
	);
}

sf_snd sf_highshelf(sf_snd snd, float freq, float Q, float gain){ // WebAudio hardcodes Q=1
	float nyquist = snd->rate * 0.5f;
	freq /= nyquist;

	// TODO: figure out what happens when Q <= 0 or Q >= 1

	if (freq >= 1.0f)
		return biquad_filter(snd, 1, 0, 0, 0, 0); // copy sample directly

	float A = powf(10.0f, gain * 0.025f);

	if (freq <= 0.0f)
		return biquad_filter(snd, A * A, 0, 0, 0, 0); // scale by A squared

	float w0    = (float)M_PI * 2.0f * freq;
	float alpha = 0.5f * sinf(w0) * sqrtf((A + 1.0f / A) * (1.0f / Q - 1.0f) + 2.0f);
	float k     = cosf(w0);
	float k2    = 2.0f * sqrtf(A) * alpha;
	float Ap1   = A + 1.0f;
	float Am1   = A - 1.0f;
	float a0inv = 1.0f / (Ap1 - Am1 * k + k2);
	return biquad_filter(snd,
		a0inv * A * (Ap1 + Am1 * k + k2),    // b0
		a0inv * -2.0f * A * (Am1 + Ap1 * k), // b1
		a0inv * A * (Ap1 + Am1 * k - k2),    // b2
		a0inv * 2.0f * (Am1 - Ap1 * k),      // a1
		a0inv * (Ap1 - Am1 * k - k2)         // a2
	);
}
