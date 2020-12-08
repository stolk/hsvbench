#ifndef HSV_H
#define HSV_H

#define CLAMP01(x) ( (x) < 0 ? 0 : ( x > 1 ? 1 : (x) ) )

static __inline__ void fast_hsv_to_rgb
(
	float h, float s, float v,
	float* rgb
)
{
	const float h6 = 6.0f * h;
	const float r = fabsf( h6 - 3.0f ) - 1.0f;
	const float g = 2.0f - fabsf( h6 - 2.0f );
	const float b = 2.0f - fabsf( h6 - 4.0f );

	const float is = 1.0f - s;
	rgb[0] = v * ( s * CLAMP01(r) + is );
	rgb[1] = v * ( s * CLAMP01(g) + is );
	rgb[2] = v * ( s * CLAMP01(b) + is );
}



static __inline__ void hsv_to_rgb
(
	float h, float s, float v,
	float *rgb
)
{
	float* r = rgb+0;
	float* g = rgb+1;
	float* b = rgb+2;
	if( s == 0.0f ) 
	{
		// achromatic (grey)
		*r = *g = *b = v;
		return;
	}
	h *= 5.9999f;			// sector 0 to 5
	int i = floor( h );
	float f = h - i;		// factorial part of h
	float p = v * ( 1.0f - s );
	float q = v * ( 1.0f - s * f );
	float t = v * ( 1.0f - s * ( 1.0f - f ) );
	switch( i ) 
	{
		case 0:
			*r = v;
			*g = t;
			*b = p;
			break;
		case 1:
			*r = q;
			*g = v;
			*b = p;
			break;
		case 2:
			*r = p;
			*g = v;
			*b = t;
			break;
		case 3:
			*r = p;
			*g = q;
			*b = v;
			break;
		case 4:
			*r = t;
			*g = p;
			*b = v;
			break;
		default:		// case 5:
			*r = v;
			*g = p;
			*b = q;
			break;
	}
}


//  Convert hue, saturation, with an implied value of 1.
static __inline__ void hs_to_rgb
(
	float h, float s,
	float *rgb
)
{
	float* r = rgb+0;
	float* g = rgb+1;
	float* b = rgb+2;
	if( s == 0.0f ) 
	{
		// achromatic (grey)
		*r = *g = *b = 1;
		return;
	}
	h *= 6.0f;			// sector 0 to 5
	int i = floor( h );
	float f = h - i;		// factorial part of h
	float p = ( 1.0f - s );
	float q = ( 1.0f - s * f );
	float t = ( 1.0f - s * ( 1.0f - f ) );
	switch( i ) 
	{
		case 0:
			*r = 1;
			*g = t;
			*b = p;
			break;
		case 1:
			*r = q;
			*g = 1;
			*b = p;
			break;
		case 2:
			*r = p;
			*g = 1;
			*b = t;
			break;
		case 3:
			*r = p;
			*g = q;
			*b = 1;
			break;
		case 4:
			*r = t;
			*g = p;
			*b = 1;
			break;
		default:		// case 5:
			*r = 1;
			*g = p;
			*b = q;
			break;
	}
}


static __inline__ float hue_to_rgb
(
	float p,
	float q,
	float t
)
{
	if ( t < 0 ) t += 1;
	if ( t > 1 ) t -= 1;
	if ( t < 1/6.0f ) return p + (q - p) * 6.0f * t;
	if ( t < 1/2.0f ) return q;
	if ( t < 2/3.0f ) return p + (q - p) * (2.0f/3.0f - t) * 6.0f;
	return p;
}


static __inline__ void hsl_to_rgb
(
	float h, float s, float l,
	float* r, float* g, float* b
)
{
	if ( s == 0 )
	{
		*r = *g = *b = l; // achromatic
	}
	else
	{
		float q = l < 0.5f ? l * ( 1 + s ) : l + s - l * s;
		float p = 2 * l - q;
		*r = hue_to_rgb( p, q, h + 1/3.0f );
		*g = hue_to_rgb( p, q, h );
		*b = hue_to_rgb( p, q, h - 1/3.0f );
	}
}


static __inline__ float linear_to_srgb(float val)
{
	if (val <= 0.0031308f)
		return 12.92f * val;
	else
		return 1.055f * powf(val, 1.0f / 2.4f) - 0.055f;
}


static __inline__ float srgb_to_linear(float val)
{
	if (val < 0.04045f)
		return val * (1.0f / 12.92f);
	else
		return powf((val + 0.055f) * (1.0f / 1.055f), 2.4f);
}


#endif

