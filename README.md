# benchhsv - Benchmark different approaches to HSV->RGB conversion.

## Introduction

This code benches three different ways of doing HSV to RGB.

standard

fast

table

## Results

The standard implementation, often popping up on stackoverflow answers, uses a 6-way switch.

This branching turns out to be quite slow.
So slow, as a matter of fact, that interpolating table-lookups beats it by a factor 2, in my tests.

Although the interpolated table lookup has inaccurate results, as can be seen here.

![standard](images/out0.png "standard")
![fast](images/out1.png "fast")
![lookup](images/out2.png "lookup")

The output of the branchless version is near identical:
```
$ ppmhist diff01.ppm 
   r     g     b   	 lum 	 count
 ----- ----- ----- 	-----	-------
     0     0     0	    0	  65140
     0     0     1	    0	    163
     1     0     0	    0	    145
     0     1     0	    1	     88
```

But the lookup version is somewhat inaccurate, and has these deltas:

![diff02](images/diff02.png "Difference between standard and lookup.")

The SRGB image space makes it hard to see, but in linear space, the deltas would be more visible.

## Performance Results

```
$ time ./hsvbench bench=standard
real	0m3.745s
user	0m3.721s
sys	0m0.024s

$ time ./hsvbench bench=fast
real	0m0.655s
user	0m0.651s
sys	0m0.004s

$ time ./hsvbench bench=table
real	0m1.865s
user	0m1.849s
sys	0m0.016s

```

## Fast Implementation

The fast implementation is after this hacker news comment:
https://news.ycombinator.com/item?id=21158888

This version can actually be written as an expression, so does not even need an algorithm!

```
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
```


## Conclusion

Table look up is only faster than a naive implementation.

Branchless HSV to RGB conversion pays off big time: I measured almost a factor 6.


