#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>

typedef struct real_2d_array_t
{
	float** arr;
	float* data;
	int     rows;
	int     cols;
}real_2d;

typedef struct real_uint8_array_t
{
	uint8_t** arr;
	uint8_t* data;
	int     rows;
	int     cols;
}uint8_2d;

typedef struct int_vector_t
{
	float* data;
	int    size;
}int_vec;

real_2d* create_real_2d(int rows, int cols);
void      destroy_real_2d(real_2d** rd);

uint8_2d* create_uint8_2d(int rows, int cols);
void      destroy_uint8_2d(uint8_2d** rd);

int_vec* create_int_vec(int size);
void       destroy_int_vec(int_vec** rv);

real_2d* uint8_to_real(uint8_t* img, int w, int h);
uint8_2d* real_to_uint8(real_2d* img);

float get_real_2d_max(real_2d* rd);
void normalize_real_2d(real_2d* rd, float max_val);
void normalized_to_unnormalize(real_2d* rd);
void canny(uint8_t* data, int w, int h, uint8_t** edge);