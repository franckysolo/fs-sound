/**
 * Create sound in C
 * @see http://soundfile.sapp.org/doc/WaveFormat/
 *
 * Base on article linux magazine n° 190
 * @see https://github.com/GLMF/GLMF190
 */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define PI 3.1415926535898
#define MAX_AMP 32767

// Taux d'echantillonnage
unsigned int rate;
// taille
unsigned long int size;
// canaux
double *channel_left, *channel_right;
// Durée de la piste sonnore
double duration;

void write_little_endian(FILE *file, unsigned int bytes, int size) {

	unsigned char low;

	while (size > 0) {
		low = bytes & 0x000000FF;
		fwrite(&low, 1, 1, file);
		bytes = bytes >> 8;
		size -= 1;
	}
}

void write_wave_header(FILE *file) {

	unsigned short int 	channel_count 		= 2; // stéréeo
	unsigned short int 	bits_per_sample 	= 16; // 16 bits
	unsigned long int 	sub_chunk_size;
	unsigned long int 	byte_rate;
	unsigned long int 	block_align;

	rate 			= 44100;	// 44100 khz
	duration 		= 5;  // 5 secondes

	size 			= rate * duration;
	sub_chunk_size 	= (bits_per_sample / 8) * channel_count * size;
	byte_rate 		= rate * channel_count * (bits_per_sample / 8);
	block_align 	= channel_count * (bits_per_sample / 8);

	// Allocation de mémoire pour les canaux de données
	channel_left 	= calloc(size, sizeof(double));
	channel_right 	= calloc(size, sizeof(double));

	// THE RIFF HEADER
	fwrite("RIFF", 4, 1, file);
	write_little_endian(file, 36 + sub_chunk_size, 4);
	fwrite("WAVE", 4, 1, file);
	fwrite("fmt ", 4, 1, file);

	// 16 for PCM.  This is the size of the rest of the Subchunk which follows this number
	write_little_endian(file, 16, 4); // offset = 16 ; Size = 4
	//  PCM = 1 (i.e. Linear quantization) Values other than 1 indicate some form of compression.
	write_little_endian(file, 1, 2);
	// NumChannels      Mono = 1, Stereo = 2, etc.
	write_little_endian(file, channel_count, 2);
	// SampleRate       8000, 44100, etc.
	write_little_endian(file, rate, 4);
	//  ByteRate   == SampleRate * NumChannels * BitsPerSample/8
	write_little_endian(file, byte_rate, 4);
	// BlockAlign       == NumChannels * BitsPerSample/8
	write_little_endian(file, block_align, 2);
	// BitsPerSample    8 bits = 8, 16 bits = 16, etc.
	write_little_endian(file, bits_per_sample, 2);

	// The data
	fwrite("data", 4, 1, file);
	// Subchunk2Size    == NumSamples * NumChannels * BitsPerSample/8
	write_little_endian(file, size * channel_count * (bits_per_sample / 8), 4);
}

void write_wave_data(FILE *file) {

	unsigned int i;
	double maxi = 1e-16;

	for (i = 0; i < size; i+=1) {
		if (fabs(channel_left[i]) > maxi)  maxi = fabs(channel_left[i]);
		if (fabs(channel_right[i]) > maxi) maxi = fabs(channel_right[i]);
	}

	for (i = 0; i < size; i+=1) {
		write_little_endian(file, (int)(channel_left[i] / maxi * MAX_AMP), 2);
		write_little_endian(file, (int)(channel_right[i] / maxi * MAX_AMP), 2);
	}
}

void create_sound(double t1, double t2, double f, double amp) {

	unsigned int i, min, max;
	double omega 	= 2 * PI * f;
	double t 		= 0;
	double dt  		= 1.0 / rate;
	double phi		= 0;

	min = (unsigned int)(t1 * rate);
	max = (unsigned int)(t2 * rate);

	for (i = min; i < max; i+=1) {
		channel_left[i]  = channel_left[i] + amp * sin(omega * t + phi);
		channel_right[i] = channel_right[i] + amp * sin(omega * t + phi);
		t += dt;
	}
}

int main () {
	FILE *file = fopen("c-audio.wav", "wb");
	write_wave_header(file);
	create_sound(0.0, 5.0, 440.0, 1.0);
	write_wave_data(file);
	fclose(file);
}
