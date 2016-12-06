// (c) Copyright 2016, Sean Connelly (@voidqk), http://syntheti.cc
// MIT License
// Project Home: https://github.com/voidqk/sndfilter

#include "wav.h"
#include <stdio.h>
#include <stdint.h>

// read an unsigned 32-bit integer in little endian format
static inline uint32_t read_u32le(FILE *fp){
	uint32_t b1 = fgetc(fp);
	uint32_t b2 = fgetc(fp);
	uint32_t b3 = fgetc(fp);
	uint32_t b4 = fgetc(fp);
	return b1 | (b2 << 8) | (b3 << 16) | (b4 << 24);
}

// read an unsigned 16-bit integer in little endian format
static inline uint16_t read_u16le(FILE *fp){
	uint16_t b1 = fgetc(fp);
	uint16_t b2 = fgetc(fp);
	return b1 | (b2 << 8);
}

// write an unsigned 32-bit integer in little endian format
static inline void write_u32le(FILE *fp, uint32_t v){
	fputc(v & 0xFF, fp);
	fputc((v >> 8) & 0xFF, fp);
	fputc((v >> 16) & 0xFF, fp);
	fputc((v >> 24) & 0xFF, fp);
}

// write an unsigned 16-bit integer in little endian format
static inline void write_u16le(FILE *fp, uint16_t v){
	fputc(v & 0xFF, fp);
	fputc((v >> 8) & 0xFF, fp);
}

// load a WAV file (returns NULL for error)
sf_snd sf_wavload(const char *file){
	FILE *fp = fopen(file, "rb");
	if (fp == NULL)
		return NULL;

	uint32_t riff = read_u32le(fp);
	if (riff != 0x46464952){ // 'RIFF'
		fclose(fp);
		return NULL;
	}

	uint32_t filesize = read_u32le(fp); // don't really care about this

	uint32_t wave = read_u32le(fp);
	if (wave != 0x45564157){ // 'WAVE'
		fclose(fp);
		return NULL;
	}

	// start reading chunks
	sf_snd s = sf_snd_new();
	if (s == NULL){
		fclose(fp);
		return NULL;
	}

	bool found_fmt = false;
	uint16_t audioformat;
	uint16_t numchannels;
	uint32_t samplerate;
	uint16_t bps;
	while (!feof(fp)){
		uint32_t chunkid = read_u32le(fp);
		uint32_t chunksize = read_u32le(fp);
		if (chunkid == 0x20746D66){ // 'fmt '

			// confirm we haven't already processed the fmt chunk
			if (found_fmt || chunksize < 16){
				sf_snd_free(s);
				fclose(fp);
				return NULL;
			}
			found_fmt = true;

			// load the fmt information
			audioformat = read_u16le(fp);
			numchannels = read_u16le(fp);
			samplerate  = read_u32le(fp);
			read_u32le(fp); // byte rate, ignored
			read_u16le(fp); // block align, ignored
			bps         = read_u16le(fp);

			// only support 1/2-channel 16-bit samples
			if (audioformat != 1 || bps != 16 || (numchannels != 1 && numchannels != 2)){
				sf_snd_free(s);
				fclose(fp);
				return NULL;
			}

			// save the sample rate for later
			s->rate = samplerate;

			// skip ahead of the rest of the fmt chunk
			if (chunksize > 16)
				fseek(fp, chunksize - 16, SEEK_CUR);
		}
		else if (chunkid == 0x61746164){ // 'data'

			// confirm we've already processed the fmt chunk
			if (!found_fmt){
				sf_snd_free(s);
				fclose(fp);
				return NULL;
			}

			// confirm chunk size is evenly divisible by bytes per sample
			if ((chunksize % (numchannels * bps / 8)) != 0){
				sf_snd_free(s);
				fclose(fp);
				return NULL;
			}

			// calculate the number of samples based on the chunk size and allocate the space
			int scount = chunksize / (numchannels * bps / 8);
			s->size = scount;
			s->samples = malloc(sizeof(sf_sample_st) * scount);
			if (s->samples == NULL){
				sf_snd_free(s);
				fclose(fp);
				return NULL;
			}

			// read the data and convert to stereo floating point
			int16_t L, R;
			for (int i = 0; i < scount; i++){
				// read the sample
				L = (int16_t)read_u16le(fp);
				if (numchannels == 1)
					R = L; // expand to stereo
				else
					R = (int16_t)read_u16le(fp);

				// convert the sample to floating point
				if (L < 0)
					s->samples[i].L = (float)L / 32768.0f;
				else
					s->samples[i].L = (float)L / 32767.0f;
				if (R < 0)
					s->samples[i].R = (float)R / 32768.0f;
				else
					s->samples[i].R = (float)R / 32767.0f;
			}

			// we've loaded the wav data, so just return now
			fclose(fp);
			return s;
		}
		else{ // skip an unknown chunk
			if (chunksize > 0)
				fseek(fp, chunksize, SEEK_CUR);
		}
	}

	// didn't find data chunk, so fail
	sf_snd_free(s);
	fclose(fp);
	return NULL;
}

static float clampf(float v, float min, float max){
	return v < min ? min : (v > max ? max : v);
}

// save a WAV file (returns false for error)
bool sf_wavsave(sf_snd s, const char *file){
	FILE *fp = fopen(file, "wb");
	if (fp == NULL)
		return false;

	// calculate the different file sizes based on sample size
	uint32_t size2 = s->size * 4; // total bytes of data
	uint32_t sizeall = size2 + 36; // total file size minus 8
	if (s->size > size2 || s->size > sizeall || size2 > sizeall)
		return false; // sample too large

	write_u32le(fp, 0x46464952);  // 'RIFF'
	write_u32le(fp, sizeall);     // rest of file size
	write_u32le(fp, 0x45564157);  // 'WAVE'
	write_u32le(fp, 0x20746D66);  // 'fmt '
	write_u32le(fp, 16);          // size of fmt chunk
	write_u16le(fp, 1);           // audio format
	write_u16le(fp, 2);           // stereo
	write_u32le(fp, s->rate);     // sample rate
	write_u32le(fp, s->rate * 4); // bytes per second
	write_u16le(fp, 4);           // block align
	write_u16le(fp, 16);          // bits per sample
	write_u32le(fp, 0x61746164);  // 'data'
	write_u32le(fp, size2);       // size of data chunk

	// convert the sample to stereo 16-bit, and write to file
	for (int i = 0; i < s->size; i++){
		float L = clampf(s->samples[i].L, -1, 1);
		float R = clampf(s->samples[i].R, -1, 1);
		int16_t Lv, Rv;
		if (L < 0)
			Lv = (int16_t)(L * 32768.0f);
		else
			Lv = (int16_t)(L * 32767.0f);
		if (R < 0)
			Rv = (int16_t)(R * 32768.0f);
		else
			Rv = (int16_t)(R * 32767.0f);
		write_u16le(fp, (uint16_t)Lv);
		write_u16le(fp, (uint16_t)Rv);
	}

	fclose(fp);
	return true;
}
