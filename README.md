# fasthsv - Benchmark different approaches to HSV->RGB conversion.

## Introduction

This code benches three different ways of doing HSV to RGB.

standard

fast

table

## Performance Results

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

But the lookup version is inaccurate, and has these deltas:
![diff02](images/diff02.png "Difference between standard and lookup.")

