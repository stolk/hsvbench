#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "hsv.h"


static float table[17][17][3];	// [hue][sat][channel]

static void init_table(void)
{
	for ( int h=0; h<17; ++h )
	{
		const float hue = h / 16.001f;
		for ( int s=0; s<17; ++s )
		{
			const float sat = s / 16.0f;
			const float val = 1.0f;
			hs_to_rgb( hue, sat, table[ h ][ s ] );
		}
	}
}


__inline__ void hue_sat_val_lookup( uint8_t h, uint8_t s, float value, float* rgb )
{
	const int hidx = h / 16;
	const float hoff = (h - hidx * 16) / 16.0f;
	const int sidx = s / 16;
	const float soff = (s - sidx * 16) / 16.0f;
	float* v0 = table[hidx+0][sidx+0];
	float* v1 = table[hidx+0][sidx+1];
	float* v2 = table[hidx+1][sidx+0];
	float* v3 = table[hidx+1][sidx+1];
	const float hoffc = 1.0 - hoff;
	const float soffc = 1.0 - soff;
	const float r01 = soffc * v0[0] + soff * v1[0];
	const float g01 = soffc * v0[1] + soff * v1[1];
	const float b01 = soffc * v0[2] + soff * v1[2];
	const float r23 = soffc * v2[0] + soff * v3[0];
	const float g23 = soffc * v2[1] + soff * v3[1];
	const float b23 = soffc * v2[2] + soff * v3[2];
	rgb[0] = value * ( hoffc * r01 + hoff * r23 );
	rgb[1] = value * ( hoffc * g01 + hoff * g23 );
	rgb[2] = value * ( hoffc * b01 + hoff * b23 );
}


static void write_ppm( const char* name, int w, int h, uint8_t* im )
{
	FILE* f = fopen( name, "wb" );
	assert( f );
	fprintf( f, "P6 %d %d\n%d\n", w, h, 255 );
	fwrite( im, w*h*3, 1, f );
	fclose( f );
}


uint8_t im_hsv8[1024*1024][3];
float   im_hsv [1024*1024][3];
float   im_rgb [1024*1024][3];


static void bench_standard(int count)
{
	for (int run=0; run<count; ++run )
	{
		for ( int i=0; i<1024*1024; ++i )
		{
			hsv_to_rgb( im_hsv[ i ][ 0 ], im_hsv[ i ][ 1 ], im_hsv[ i ][ 2 ],  im_rgb[ i ] );
		}
	}
}


static void bench_table(int count)
{
	
	for (int run=0; run<count; ++run )
	{
		for ( int i=0; i<1024*1024; ++i )
		{
			hue_sat_val_lookup( im_hsv8[ i ][ 0 ], im_hsv8[ i ][ 1 ], im_hsv[ i ][ 2 ], im_rgb[ i ] );
		}
	}
}


static void bench_fast(int count)
{
	for (int run=0; run<count; ++run )
	{
		for ( int i=0; i<1024*1024; ++i )
		{
			fast_hsv_to_rgb( im_hsv[ i ][ 0 ], im_hsv[ i ][ 1 ], im_hsv[ i ][ 2 ],  im_rgb[ i ] );
		}
	}
}


static int opt_ppm=0;
static const char* opt_bench="none";

int main( int argc, char* argv[] )
{
	for (int i=1; i<argc; ++i)
	{
		const char* a = argv[i];
		if ( !strncmp( a, "ppm=", 4 ) ) opt_ppm = atoi( a+4);
		if ( !strncmp( a, "bench=", 6 ) ) opt_bench = a+6;
	}
	const int do_fast     = !strncmp( opt_bench, "fast",     4 );
	const int do_table    = !strncmp( opt_bench, "table",    5 );
	const int do_standard = !strncmp( opt_bench, "standard", 8 );

	init_table();

	if ( opt_ppm )
	{
		uint8_t* im0 = (uint8_t*) malloc(256*256*3);
		uint8_t* im1 = (uint8_t*) malloc(256*256*3);
		uint8_t* im2 = (uint8_t*) malloc(256*256*3);

		uint8_t* writer0 = im0;
		uint8_t* writer1 = im1;
		uint8_t* writer2 = im2;

		for ( int h=0; h<256; h++ )
		{
			const float hue = h / 255.0f;
			for ( int s=0; s<256; s++ )
			{
				const float sat = s / 255.0f;
				float val = 1.0f;
				float sx = hue - 0.5f;
				float sy = sat - 0.5f;
				float rr = sx*sx + sy*sy;
				if ( rr > 0.37f*0.37f && rr < 0.42f*0.42f )
					val = ( fabsf( atan2f( sy, sx ) ) / M_PI );
				float colA[3];
				hsv_to_rgb( hue, sat, val, colA );
				writer0[0] = (uint8_t) (colA[0] * 255.999f);
				writer0[1] = (uint8_t) (colA[1] * 255.999f);
				writer0[2] = (uint8_t) (colA[2] * 255.999f);
				writer0 += 3;
				float colB[3];
				fast_hsv_to_rgb( hue, sat, val, colB );
				writer1[0] = (uint8_t) (colB[0] * 255.999f);
				writer1[1] = (uint8_t) (colB[1] * 255.999f);
				writer1[2] = (uint8_t) (colB[2] * 255.999f);
				writer1 += 3;
				float colC[3];
				hue_sat_val_lookup( h, s, val, colC );
				writer2[0] = (uint8_t) (colC[0] * 255.999f);
				writer2[1] = (uint8_t) (colC[1] * 255.999f);
				writer2[2] = (uint8_t) (colC[2] * 255.999f);
				writer2 += 3;
			}
		}
		fprintf( stderr, "Writing images...\n" );
		write_ppm( "out0.ppm", 256, 256, im0 );
		write_ppm( "out1.ppm", 256, 256, im1 );
		write_ppm( "out2.ppm", 256, 256, im2 );
	}

	for (int i=0; i<1024*1024; ++i)
	{
		im_hsv8[i][0] = rand() & 0xff;
		im_hsv8[i][1] = rand() & 0xff;
		im_hsv8[i][2] = rand() & 0xff;
		im_hsv [i][0] = im_hsv8[i][0] / 255.0f;
		im_hsv [i][1] = im_hsv8[i][1] / 255.0f;
		im_hsv [i][2] = im_hsv8[i][2] / 255.0f;
	}

	if ( !do_fast && !do_table && !do_standard )
		return 0;

	fprintf( stderr, "Benching...\n" );
	const int numiter = 300;
	if ( do_fast )
		bench_fast( numiter );
	if ( do_table )
		bench_table( numiter );
	if ( do_standard )
		bench_standard( numiter );

	return 0;
}

