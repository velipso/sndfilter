// (c) Copyright 2016, Sean Connelly (@velipso), https://sean.cm
// MIT License
// Project Home: https://github.com/velipso/sndfilter

// dynamics compressor based on WebAudio specification:
//   https://webaudio.github.io/web-audio-api/#the-dynamicscompressornode-interface

#ifndef SNDFILTER_COMPRESSOR__H
#define SNDFILTER_COMPRESSOR__H

#include "sf_common.h"
#include "snd.h"

// dynamic range compression is a complex topic with many different algorithms
//
// this API works by first initializing an sf_compressor_state_st structure, then using it to
// process a sample in chunks
//
// for example, say you're processing a stream in 128 samples per chunk:
//
//   sf_compressor_state_st simplecomp;
//   sf_simplecomp(&simplecomp, 48000, 5, -24, 30, 12, 0.003f, 0.250f);
//
//   for each 128 length sample:
//     sf_compressor_process(&simplecomp, 128, input, output);
//
// notice that sf_compressor_process will change a lot of the member variables inside of the state
// structure, since these values must be carried over across chunk boundaries
//
// also notice that the choice to divide the sound into chunks of 128 samples is completely
// arbitrary from the compressor's perspective, however, the size should be divisible by the SPU
// value below (defaults to 32):

// maximum number of samples in the delay buffer
#define SF_COMPRESSOR_MAXDELAY   1024

// samples per update; the compressor works by dividing the input chunks into even smaller sizes,
// and performs heavier calculations after each mini-chunk to adjust the final envelope
#define SF_COMPRESSOR_SPU        32

// not sure what this does exactly, but it is part of the release curve
#define SF_COMPRESSOR_SPACINGDB  5.0f

typedef struct {
	// user can read the metergain state variable after processing a chunk to see how much dB the
	// compressor would have liked to compress the sample; the meter values aren't used to shape the
	// sound in any way, only used for output if desired
	float metergain;

	// everything else shouldn't really be mucked with unless you read the algorithm and feel
	// comfortable
	float meterrelease;
	float threshold;
	float knee;
	float linearpregain;
	float linearthreshold;
	float slope;
	float attacksamplesinv;
	float satreleasesamplesinv;
	float wet;
	float dry;
	float k;
	float kneedboffset;
	float linearthresholdknee;
	float mastergain;
	float a; // adaptive release polynomial coefficients
	float b;
	float c;
	float d;
	float detectoravg;
	float compgain;
	float maxcompdiffdb;
	int delaybufsize;
	int delaywritepos;
	int delayreadpos;
	sf_sample_st delaybuf[SF_COMPRESSOR_MAXDELAY]; // predelay buffer
} sf_compressor_state_st;

SF_API_BEGIN

// populate a compressor state with all default values
void sf_defaultcomp(sf_compressor_state_st *state, int rate);

// populate a compressor state with simple parameters
void sf_simplecomp(sf_compressor_state_st *state,
	int rate,        // input sample rate (samples per second)
	float pregain,   // dB, amount to boost the signal before applying compression [0 to 100]
	float threshold, // dB, level where compression kicks in [-100 to 0]
	float knee,      // dB, width of the knee [0 to 40]
	float ratio,     // unitless, amount to inversely scale the output when applying comp [1 to 20]
	float attack,    // seconds, length of the attack phase [0 to 1]
	float release    // seconds, length of the release phase [0 to 1]
);

// populate a compressor state with advanced parameters
void sf_advancecomp(sf_compressor_state_st *state,
	// these parameters are the same as the simple version above:
	int rate, float pregain, float threshold, float knee, float ratio, float attack, float release,
	// these are the advanced parameters:
	float predelay,     // seconds, length of the predelay buffer [0 to 1]
	float releasezone1, // release zones should be increasing between 0 and 1, and are a fraction
	float releasezone2, //  of the release time depending on the input dB -- these parameters define
	float releasezone3, //  the adaptive release curve, which is discussed in further detail in the
	float releasezone4, //  demo: adaptive-release-curve.html
	float postgain,     // dB, amount of gain to apply after compression [0 to 100]
	float wet           // amount to apply the effect [0 completely dry to 1 completely wet]
);

// this function will process the input sound based on the state passed
// the input and output buffers should be the same size
void sf_compressor_process(sf_compressor_state_st *state, int size, sf_sample_st *input,
	sf_sample_st *output);

SF_API_END

#endif // SNDFILTER_COMPRESSOR__H
