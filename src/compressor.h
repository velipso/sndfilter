// (c) Copyright 2016, Sean Connelly (@voidqk), http://syntheti.cc
// MIT License
// Project Home: https://github.com/voidqk/sndfilter

// dynamics compressor based on WebAudio specification:
//   https://webaudio.github.io/web-audio-api/#the-dynamicscompressornode-interface

#ifndef SNDFILTER_COMPRESSOR__H
#define SNDFILTER_COMPRESSOR__H

#include "types.h"

typedef struct {
	float threshold;
	float knee;
	float ratio;
	float attack;
	float release;
	float predelay;
	float releasezone1;
	float releasezone2;
	float releasezone3;
	float releasezone4;
	float postgain;
	float wet;
} sf_compressor_params_st;

extern const sf_compressor_params_st sf_compressor_defaults;

sf_snd sf_compressor(sf_snd snd, sf_compressor_params_st params);

#endif // SNDFILTER_COMPRESSOR__H
