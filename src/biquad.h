// (c) Copyright 2016, Sean Connelly (@voidqk), http://syntheti.cc
// MIT License
// Project Home: https://github.com/voidqk/sndfilter

// biquad filtering based on WebAudio specification:
//  https://webaudio.github.io/web-audio-api/#the-biquadfilternode-interface

#ifndef SNDFILTER_BIQUAD__H
#define SNDFILTER_BIQUAD__H

#include "types.h"

sf_snd sf_lowpass(sf_snd snd, float cutoff, float resonance);
sf_snd sf_highpass(sf_snd snd, float cutoff, float resonance);
sf_snd sf_bandpass(sf_snd snd, float freq, float Q);
sf_snd sf_notch(sf_snd snd, float freq, float Q);
sf_snd sf_peaking(sf_snd snd, float freq, float Q, float gain);
sf_snd sf_allpass(sf_snd snd, float freq, float Q);
sf_snd sf_lowshelf(sf_snd snd, float freq, float Q, float gain);
sf_snd sf_highshelf(sf_snd snd, float freq, float Q, float gain);

#endif // SNDFILTER_BIQUAD__H
