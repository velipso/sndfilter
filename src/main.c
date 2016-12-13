// (c) Copyright 2016, Sean Connelly (@voidqk), http://syntheti.cc
// MIT License
// Project Home: https://github.com/voidqk/sndfilter

// main entry point, used as a simple demo of the features of the library

#include "wav.h"
#include "biquad.h"
#include "compressor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int printabout(){
	printf(
		"sndfilter - simple demonstrations of common sound filters\n"
		"(c) Copyright 2016, Sean Connelly (@voidqk), http://syntheti.cc\n"
		"MIT License\n"
		"Project Home: https://github.com/voidqk/sndfilter\n");
	return 0;
}

static int printhelp(){
	printabout();
	printf("\n"
		"Usage:\n"
		"  sndfilter input.wav output.wav <filter> <...>\n"
		"\n"
		"Where:\n"
		"  input.wav    Input WAV file to process\n"
		"  output.wav   Output WAV file of filtered results\n"
		"  <filter>     One of the available filters (see below)\n"
		"  <...>        Additional parameters for the particular filter\n"
		"\n"
		"  Filters:\n"
		"    lowpass     Passes low frequencies through and dampens high frequencies\n"
		"    highpass    Passes high frequencies through and dampens low frequencies\n"
		"    bandpass    Passes a band of frequencies through and dampens outside them\n"
		"    notch       Dampens the overtones surrounding a frequency\n"
		"    allpass     Phase shifts based on frequency (usually not audible)\n"
		"    peaking     Adds gain to the overtones surrounding a frequency\n"
		"    lowshelf    Adds gain to lower frequencies\n"
		"    highshelf   Adds gain to higher frequencies\n"
		"    compressor  Dyanmic range compression, usually to make sounds louder\n"
		"\n"
		"  Filter Details:\n"
		"    lowpass <cutoff> <resonance>\n"
		"      cutoff     Frequency of the filter cutoff (Hz)\n"
		"      resonance  Boost applied to overtones near cutoff\n"
		"\n"
		"    highpass <cutoff> <resonance>\n"
		"      cutoff     Frequency of the filter cutoff (Hz)\n"
		"      resonance  Boost applied to overtones near cutoff\n"
		"\n"
		"    bandpass <freq> <Q>\n"
		"      freq       Center freqency of the overtones passed through (Hz)\n"
		"      Q          Inverse of the fractional bandwidth\n"
		"\n"
		"    notch <freq> <Q>\n"
		"      freq       Center frequency of the attenuated overtones (Hz)\n"
		"      Q          Inverse of the fractional bandwidth\n"
		"\n"
		"    peaking <freq> <Q> <gain>\n"
		"      freq       Center frequency where the gain occurs (Hz)\n"
		"      Q          Inverse of the fractional bandwidth\n"
		"      gain       Amount of gain (dB)\n"
		"\n"
		"    allpass <freq> <Q>\n"
		"      freq       Frequency where the phase shift crosses 90 degrees (Hz)\n"
		"      Q          Inverse of the fractional bandwidth\n"
		"\n"
		"    lowshelf <freq> <Q> <gain>\n"
		"      freq       Frequency where the low shelf stops producing gain (Hz)\n"
		"      Q          Inverse of the fractional bandwidth\n"
		"      gain       Amount of gain of the shelf (dB)\n"
		"\n"
		"    highshelf <freq> <Q> <gain>\n"
		"      freq       Frequency where the high shelf stops producing gain (Hz)\n"
		"      Q          Inverse of the fractional bandwidth\n"
		"      gain       Amount of gain of the shelf (dB)\n"
		"\n"
		"    compressor <pregain> <threshold> <knee> <ratio> <attack> <release>\n"
		"      pregain    Decibel amount to perform gain before compression (0 to 40)\n"
		"      threshold  Decibel level that triggers the compression (-100 to 0)\n"
		"      knee       Decibel width of the knee (0 to 40)\n"
		"      ratio      Ratio of compression after the threshold (1 to 20)\n"
		"      attack     Seconds for the compression to kick in (0 to 1)\n"
		"      release    Seconds for the compression to release (0 to 1)\n");
	return 0;
}

static inline bool getargs(int argc, char **argv, int size, float *params){
	if (argc < 4 + size)
		return false;
	for (int i = 0; i < size; i++)
		params[i] = atof(argv[4 + i]);
	return true;
}

static inline int badargs(const char *filter){
	fprintf(stderr, "Error: Bad arguments for %s\n", filter);
	return 1;
}

static inline int biquad(sf_snd input_snd, sf_biquad_state_st *state, const char *output){
	sf_snd output_snd = sf_snd_new(input_snd->size, input_snd->rate, false);
	if (output_snd == NULL){
		sf_snd_free(input_snd);
		fprintf(stderr, "Error: Failed to apply filter\n");
		return 1;
	}

	// process the filter in one sweep
	sf_biquad_process(state, input_snd->size, input_snd->samples, output_snd->samples);

	bool res = sf_wavsave(output_snd, output);
	sf_snd_free(input_snd);
	sf_snd_free(output_snd);
	if (!res){
		fprintf(stderr, "Error: Failed to save WAV: %s\n", output);
		return 1;
	}
	return 0;
}

static inline int compressor(sf_snd input_snd, sf_compressor_state_st *state, const char *output){
	sf_snd output_snd = sf_snd_new(input_snd->size, input_snd->rate, true);
	if (output_snd == NULL){
		sf_snd_free(input_snd);
		fprintf(stderr, "Error: Failed to apply filter\n");
		return 1;
	}

	// process the compressor in one sweep
	sf_compressor_process(state, input_snd->size, input_snd->samples, output_snd->samples);

	// note that the compressor does not output one sample per input sample, because the compressor
	// works in subchunks of 32 samples (this is defined via SF_COMPRESSOR_SPU in compressor.c)
	//
	// that means the output size will be floor(input_size / 32) * 32

	bool res = sf_wavsave(output_snd, output);
	sf_snd_free(input_snd);
	sf_snd_free(output_snd);
	if (!res){
		fprintf(stderr, "Error: Failed to save WAV: %s\n", output);
		return 1;
	}
	return 0;
}

int main(int argc, char **argv){
	if (argc < 4)
		return printhelp();

	const char *input  = argv[1];
	const char *output = argv[2];
	const char *filter = argv[3];

	sf_snd input_snd = sf_wavload(input);
	if (input_snd == NULL){
		fprintf(stderr, "Error: Failed to load WAV: %s\n", input);
		return 1;
	}

	float params[6];
	sf_biquad_state_st bq_state;
	if (strcmp(filter, "lowpass") == 0){
		if (!getargs(argc, argv, 2, params))
			return badargs(filter);
		sf_lowpass(&bq_state, input_snd->rate, params[0], params[1]);
		return biquad(input_snd, &bq_state, output);
	}
	else if (strcmp(filter, "highpass") == 0){
		if (!getargs(argc, argv, 2, params))
			return badargs(filter);
		sf_highpass(&bq_state, input_snd->rate, params[0], params[1]);
		return biquad(input_snd, &bq_state, output);
	}
	else if (strcmp(filter, "bandpass") == 0){
		if (!getargs(argc, argv, 2, params))
			return badargs(filter);
		sf_bandpass(&bq_state, input_snd->rate, params[0], params[1]);
		return biquad(input_snd, &bq_state, output);
	}
	else if (strcmp(filter, "notch") == 0){
		if (!getargs(argc, argv, 2, params))
			return badargs(filter);
		sf_notch(&bq_state, input_snd->rate, params[0], params[1]);
		return biquad(input_snd, &bq_state, output);
	}
	else if (strcmp(filter, "peaking") == 0){
		if (!getargs(argc, argv, 3, params))
			return badargs(filter);
		sf_peaking(&bq_state, input_snd->rate, params[0], params[1], params[2]);
		return biquad(input_snd, &bq_state, output);
	}
	else if (strcmp(filter, "allpass") == 0){
		if (!getargs(argc, argv, 2, params))
			return badargs(filter);
		sf_allpass(&bq_state, input_snd->rate, params[0], params[1]);
		return biquad(input_snd, &bq_state, output);
	}
	else if (strcmp(filter, "lowshelf") == 0){
		if (!getargs(argc, argv, 3, params))
			return badargs(filter);
		sf_lowshelf(&bq_state, input_snd->rate, params[0], params[1], params[2]);
		return biquad(input_snd, &bq_state, output);
	}
	else if (strcmp(filter, "highshelf") == 0){
		if (!getargs(argc, argv, 3, params))
			return badargs(filter);
		sf_highshelf(&bq_state, input_snd->rate, params[0], params[1], params[2]);
		return biquad(input_snd, &bq_state, output);
	}
	else if (strcmp(filter, "compressor") == 0){
		if (!getargs(argc, argv, 6, params))
			return badargs(filter);
		sf_compressor_state_st cm_state;
		sf_simplecomp(&cm_state, input_snd->rate, params[0], params[1], params[2], params[3],
			params[4], params[5]);
		return compressor(input_snd, &cm_state, output);
	}

	printhelp();
	fprintf(stderr, "Error: Bad filter \"%s\"\n", filter);
	return 1;
}
