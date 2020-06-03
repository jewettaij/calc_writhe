[![C++11](https://img.shields.io/badge/C%2B%2B-11-blue.svg)](https://isocpp.org/std/the-standard)
[![GitHub code size in bytes](https://img.shields.io/github/languages/code-size/jewettaij/calc_writhe)]()
[![License](https://img.shields.io/badge/License-MIT-green.svg)]()


calc_writhe
===========

## Description

This program is a small utility which reads a file containing the
coordinates of a *closed* curve in 3 dimensions (from the standard input),
and calculates its [writhe](https://en.wikipedia.org/wiki/Writhe)
using a discretized version of the Gauss
[linking number integral](https://en.wikipedia.org/wiki/Writhe#Writhe_of_a_closed_curve).
![linking number integral](https://wikimedia.org/api/rest_v1/media/math/render/svg/c38baa2103b96fdb9d91d508e8c4d179f4773881)

The file should be in 3-column ASCII format (whitespace delimited).
Points in the curve need not be equally spaced (although this is reccommended).

This code was designed to calculate the writhe of moderately large curves
containing 10^5-10^6 points, and it does so by computing the discretized
double-integral in a straightforward way.
*(O(n^2))*.  Optimization strategies to reduce computation time are discussed below.)
Multiprocessor support is implemented using
[OpenMP.](https://en.wikipedia.org/wiki/OpenMP)

*(The double-for-loop that calculates the writhe is located in this
 [header file](src/calc_writhe.hpp).
 The CalcWrithe() template function can accept coordinates in 
 any C/C++ array-like container which supports [i][j] indexing.)*

## Example usage

```
   calc_writhe < curve_coordinates.raw
```

### Progress updates (experimental)

If the curve contains many points, calculating the double-sum can be slow.
You can monitor the summation progress this way:
```
   calc_writhe -progress 10 < curve_coordinates.raw
```
This will cause the program to print progress updates every 10 seconds.
*(However the results may be difficult to understand.
  I was too lazy to separate the output from different processors,
       so unless you compiled the serial version, expect to see multiple
       conflicting number.  The progress can be estimated by taking the lowest
       of these numbers, multiplying it by the number of processors, 
       and dividing it by the length of the curve.
       This is easy to fix, but I don't want to spend time on it.
       -Andrew 2020-5-22)*

### Suggestions for numerical stability

The writhe is challenging to calculate.
It can be very sensitive to numerical integration error
(due to the finite distance between points on the curve).
Small changes in the curve which may not be visually obvious
can also have a large effect on the outcome.

To minimize this, the distance between consecutive points on the curve
should be smaller than the distance between other pairs of points on
the curve (and preferably uniform throughout the curve).
Smoothing or interpolating the curve beforehand can help.
(If it helps, a simple command line interpolation tool is available
[here](https://github.com/jewettaij/moltemplate/blob/master/moltemplate/interpolate_curve.py),
with documentation
[here](https://github.com/jewettaij/moltemplate/blob/master/doc/doc_interpolate_curve.md).)

#### *Not for use on open curves*

You must make sure the curve is closed.
If not (ie. if the two ends the curve are not nearby),
then the resulting writhe estimate will be numerically
unstable and can vary by orders of magnitude.

## Compilation

```
   cd src
   source setup_gcc.sh
   make clean
   make
```

## Development Status: *alpha*

As of 2020-5-22, **calc_writhe** has not been tested extensively,
however it is a very simple program (with two nested for-loops).
(If there are bugs, they are probably in the code that reads the text
file containing the coordinates.)

### Possible future improvements:

1) Use a higher-order integrator to improve accuracy
(ie. a 2nd-order Runge-Kutta-like integrator).
2) Use an arbitrary-precision accumulator.
3) To reduce computation time, one could merge contributions from many points
along the curve *(j,j+1,j+2,...)* when their partner *(i)* is physically far away.
4) Conversely, when the distance between *i* and *j* is low, automatically
subdivide the curve using interpolation to improve accuracy.
5) For curves which are not closed, automatically attempt to close them in
a reasonable way beforehand.  (Currently *calc_writhe* closes the two ends
directly with a single line segment, regardless of the distance between them,
or the proximity of this line segment to other parts of the curve.
This will usually cause numeric instability.)

I have no plans to implement any of these features. -A 2020-5-22

## Requirements

A C++11 compatible compiler.

## License

*calc_writhe* is available under the terms of the [MIT](LICENSE.md) license.
