/// @file     calc_writhe.hpp
/// @brief    Calculate the writhe of a curve using a discretized version
///           of Gauss' linking number integral.
///           (https://en.wikipedia.org/wiki/Writhe)
/// @author   Andrew Jewett
/// @license  MIT

#ifndef _CALC_WRITHE_HPP
#define _CALC_WRITHE_HPP

#include <cmath>
#include <cassert>
#include <chrono>
#include <iostream>

namespace calc_writhe {

// Memory management:

/// @brief  Allocate a 2-dimensional array.  (Uses row-major order.)
template<typename Entry>
void Alloc2D(size_t nrows,          //!< size of the array (number of rows)
             size_t ncols,          //!< size of the array (number of columns)
             Entry ***paaX)         //!< pointer to a 2D C-style array
{
  assert(paaX);
  *paaX = new Entry* [nrows];  //conventional 2D C array (pointer-to-pointer)
  (*paaX)[0] = new Entry [nrows * ncols];  // 1D C array (contiguous memory)
  for(size_t iy=0; iy<nrows; iy++)
    (*paaX)[iy] = (*paaX)[0] + iy*ncols;
  // The caller can access the contents using (*paaX)[i][j]
}

/// @brief  Deallocate arrays that were created using Alloc2D().
template<typename Entry>
void Dealloc2D(Entry ***paaX)  //!< pointer to a 2D C-style array
{
  if (paaX && *paaX) {
    delete [] (*paaX)[0];
    delete [] (*paaX);
    *paaX = nullptr;
  }
}


// Next: define some basic vector utilities:

template<typename Vect, typename ConstVect>
void CrossProd(Vect dest, ConstVect A, ConstVect B) {
  dest[0] = (A[1] * B[2] - B[1] * A[2]);
  dest[1] = (A[2] * B[0] - B[2] * A[0]);
  dest[2] = (A[0] * B[1] - B[0] * A[1]);
}


template<typename T, typename ConstVect>
T DotProd(ConstVect A, ConstVect B) {
  T c = 0.0;
  for (int d=0; d < 3; d++)
    c += A[d] * B[d];
  return c;
}


template<typename T, typename ConstVect>
T TripleProduct(ConstVect A, ConstVect B, ConstVect C) {
  // = (A X B) . C
  return ((A[1] * B[2] - B[1] * A[2]) * C[0] +
          (A[2] * B[0] - B[2] * A[0]) * C[1] +
          (A[0] * B[1] - B[0] * A[1]) * C[2]);
}


template<typename T, typename ConstVect>
T Length(ConstVect x) {
  T L = 0.0;
  for (int d=0; d < 3; d++)
    L += x[d] * x[d];
  return sqrt(L);
}


template<typename T, typename Vect, typename ConstVect>
void Normalize(Vect dest, ConstVect source) {
  T L = Length(source);
  for (int d=0; d < 3; d++)
    dest[d] = source[d] / L;
}


template<typename T, typename CoordArray>
T CalcWrithe(long N,
             CoordArray curve,
             double report_progress = 0)
{
  // First, precompute each line segment's length, midpoint, and direction.

  //T *L_i = new T[N]; // the length of the ith line segment  NOT_NEEDED
  T **R_i;  // the position of the center (midpoint) of the ith line segment
  T **dR_i;  // the difference in positions between the ends of the line segment

  Alloc2D(N, 3, &R_i);
  Alloc2D(N, 3, &dR_i);
  for (long i = 0; i < N; i++) {
    long ip1 = i+1;
    if (ip1 >= N)
      ip1 -= N;
    for (int d=0; d < 3; d++) {
      R_i[i][d] = 0.5*(curve[i][d] + curve[ip1][d]);
      dR_i[i][d] = (curve[ip1][d] - curve[i][d]);
    }    
    T delta[3];
    T sum = 0.0;
    for (int d=0; d < 3; d++) {
      T delta = curve[ip1][d] - curve[i][d];
      sum += delta*delta;
    }
  } //for (long i = 0; i <= N; i++)


  // Now, compute the Gauss formula (double-integral) 

  T sum = 0.0;

  #pragma omp parallel
  {

    // The following variables are private for each processor
    auto time_prev = std::chrono::high_resolution_clock::now();

    T sum_local;
    T rij[3];

    #pragma omp for collapse(1)
    for (long i = 0; i < N; i++) {

      sum_local = 0.0;

      for (long j = i+1; j < N; j++) {
        T lij = 0.0;
        for (int d=0; d < 3; d++) {
          rij[d] = R_i[i][d] - R_i[j][d];
          lij += rij[d] * rij[d];
        }
        T lij3 = lij;
        lij = sqrt(lij);
        lij3 *= lij;   // now lij3 = |rij|^3

        T delta_sum = TripleProduct<T, T const*> (dR_i[i], dR_i[j], rij);

        delta_sum /= lij3;
        sum_local += delta_sum;

      } //for (long j = 0; j <= N; j++)

      if (report_progress > 0) {
        auto time = std::chrono::high_resolution_clock::now();
        typedef std::chrono::duration<double> Seconds;
        Seconds ellapsed_time = time - time_prev;
        double ellapsed_seconds = ellapsed_time.count();
        if (ellapsed_seconds > report_progress) {
          cerr << "  (progress: " << i << "/" << N << ")" << endl;
          time_prev = time;
        }
      }

      #pragma omp critical
      {
        sum += sum_local; //add contribtions from each processor to total sum
      }

    } //for (long i = 0; i <= N; i++)
  } //#pragma omp parallel

  // Now clean up the memory we allocated earlier.
  Dealloc2D(&R_i);
  Dealloc2D(&dR_i);

  return 2.0 * sum / (4.0*M_PI);
}

} // namespace calc_writhe


#endif //#ifndef _CALC_WRITHE_HPP

