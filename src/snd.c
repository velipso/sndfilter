//
// sndfilter - Algorithms for sound filters, like reverb, lowpass, etc
// by Sean Connelly (@velipso), https://sean.fun
// Project Home: https://github.com/velipso/sndfilter
// SPDX-License-Identifier: 0BSD
//

#include "snd.h"
#include "mem.h"
#include <string.h>

sf_snd sf_snd_new(int size, int rate, bool clear){
	sf_snd snd = sf_malloc(sizeof(sf_snd_st));
	if (snd == NULL)
		return NULL;
	snd->size = size;
	snd->rate = rate;
	snd->samples = sf_malloc(sizeof(sf_sample_st) * size);
	if (snd->samples == NULL){
		sf_free(snd);
		return NULL;
	}
	if (clear && size > 0)
		memset(snd->samples, 0, sizeof(sf_sample_st) * size);
	return snd;
}

void sf_snd_free(sf_snd snd){
	sf_free(snd->samples);
	sf_free(snd);
}
