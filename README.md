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


