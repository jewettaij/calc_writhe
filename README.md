[![C++11](https://img.shields.io/badge/C%2B%2B-11-blue.svg)](https://isocpp.org/std/the-standard)
[![GitHub code size in bytes](https://img.shields.io/github/languages/code-size/jewettaij/calc_writhe)]()
[![License](https://img.shields.io/badge/License-MIT-green.svg)]()


calc_writhe
===========

## Description

This program is a small utility which reads a file containing the
coordinates of a closed curve in 3 dimensions (from the standard input),
and calculates its [writhe](https://en.wikipedia.org/wiki/Writhe)
using a discretized version of the Gauss
[linking number integral](https://en.wikipedia.org/wiki/Writhe#Writhe_of_a_closed_curve).
![linking number integral](https://wikimedia.org/api/rest_v1/media/math/render/svg/c38baa2103b96fdb9d91d508e8c4d179f4773881)

The file should be in 3-column ASCII format (whitespace delimited).
Points in the curve need not be equally spaced (although this is reccommended).

This code was designed to calculate the writhe of moderately large curves
containing 10^5-10^6 points, and it does so by computing the discretized
double-integral in a straightforward way.
Multiprocessor support is implemented using
[OpenMP.](https://en.wikipedia.org/wiki/OpenMP)

For larger curves, this code should probably be rewritten.
(For most non-pathological curves, it should be possible to merge
 contributions from many points *j,j+1,j+2...* that are sufficiently
 far away from *i* that their individual contributions are low.)

*(The double-for-loop that calculates the writhe is located in this
 [header file](src/calc_writhe.hpp).
 The CalcWrithe() template function can accept coordinates in 
 any C/C++ array-like container which supports [i][j] indexing.)*


## Example usage

```
   calc_writhe < curve_coordinaes.raw
```

### Progress updates (experimental)

If the curve contains many points, calculating the double-sum can be slow.
You can monitor the summation progress this way:
```
   calc_writhe -progress 10 < curve_coordinaes.raw
```
This will cause the program to print progress updates every 10 seconds.
*(However the results may be difficult to understand.
  I was too lazy to separate the output from different processors,
       so unless you compiled the serial version, expect to see multiple
       conflicting number.  The progress can be estimated by taking the lowest
       of these numbers, multiplying it by the number of processors, 
       and dividing it by the length of the curve.
       This is easy to fix, but I don't want to spend time on it.
       -Andrew 2020-5-20)*

## Compilation

```
   cd src
   source setup_gcc.sh
   make clean
   make
```

## Development Status: *alpha*

As of 2020-5-19, **calc_writhe** has not been tested extensively,
however it is a very simple program (with two nested for-loops).
(If there are bugs, they are probably in the code that reads the text
file containing the coordinates.)

Further improvements to numeric accuracy are possible by
1) Using a higher-order integrator
(eg. a 2nd-order Runge-Kutta-like integrator.)
2) Using an arbitrary-precision accumulator.


## Requirements

A C++11 compatible compiler.

## License

*calc_writhe* is available under the terms of the [MIT](LICENSE.md) license.
