#include <iostream>
#include <cstring>
#include <memory>
#include <chrono>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mathOperation.h>

// variables
// Catmull-Rom Spline Matrix (same as before)
float M[16] = {
    -0.5f, 1.0f, -0.5f, 0.0f,
    1.5f, -2.5f, 0.0f, 1.0f,
    -1.5f, 2.0f, 0.5f, 0.0f,
    0.5f, -0.5f, 0.0f, 0.0f
};

// basic operations
void normalize(float* v)
{
  float length = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
  if (length != 0.0f)
  {
    v[0] /= length;
    v[1] /= length;
    v[2] /= length;
  } else {
    v[0] = 0.0f;
    v[1] = 0.0f;
    v[2] = 1.0f;
  }
}

void crossProduct(const float* a, const float* b, float* result)
{
    result[0] = a[1]*b[2] - a[2]*b[1];
    result[1] = a[2]*b[0] - a[0]*b[2];
    result[2] = a[0]*b[1] - a[1]*b[0];
}

void multiplyMatrices(int m, int p, int n, const float * A, const float * B, float * C)
{
    // Multiply C = A * B, where A is a m x p matrix, and B is a p x n matrix.
    // All matrices A, B, C must be pre-allocated (say, using malloc or similar).
    // The memory storage for C must *not* overlap in memory with either A or B. 
    // That is, you **cannot** do C = A * C, or C = C * B. However, A and B can overlap, and so C = A * A is fine, as long as the memory buffer for A is not overlaping in memory with that of C.
    // Very important: All matrices are stored in **column-major** format.
    // Example. Suppose 
    //      [ 1 8 2 ]
    //  A = [ 3 5 7 ]
    //      [ 0 2 4 ]
    //  Then, the storage in memory is
    //   1, 3, 0, 8, 5, 2, 2, 7, 4. 
  for(int i=0; i<m; i++)
  {
    for(int j=0; j<n; j++)
    {
      float entry = 0.0;
      for(int k=0; k<p; k++)
        entry += A[k * m + i] * B[j * p + k];
      C[m * j + i] = entry;
    }
  }
}

// compute
void computeTangent(const float u, const float* M, const float* C, float* T)
{
  // Compute U' = [3u^2, 2u, 1, 0]
  float Uprime[4] = { 3 * u * u, 2 * u, 1.0f, 0.0f };

  // Compute U'M = U' * M (1x4 * 4x4 = 1x4)
  float UM[4];
  multiplyMatrices(1, 4, 4, Uprime, M, UM);

  // Compute t(u) = U'M * C (1x4 * 4x3 = 1x3)
  multiplyMatrices(1, 4, 3, UM, C, T);

  // Normalize the tangent vector
  normalize(T);
}

void computeBinormal(const float* tangent, const float* prevNormal, float* binormal)
{
  // Compute binormal = tangent x prevNormal
  crossProduct(tangent, prevNormal, binormal);
  normalize(binormal);
}

void computeNormal(const float* tangent, float* normal, float* binormal)
{
  // Compute normal = binormal x tangent
  crossProduct(binormal, tangent, normal);
  normalize(normal);
}

void computeNormalMatrix(const float* modelViewMatrix, float* normalMatrix)
{
  // Extract the upper-left 3x3 matrix
  float m[9] = {
    modelViewMatrix[0], modelViewMatrix[1], modelViewMatrix[2],
    modelViewMatrix[4], modelViewMatrix[5], modelViewMatrix[6],
    modelViewMatrix[8], modelViewMatrix[9], modelViewMatrix[10]
  };

  // Compute the inverse transpose
  // For orthogonal matrices, the inverse is equal to the transpose
  // Since modelViewMatrix may include scaling, we need to compute the inverse transpose
  // Here, we assume no scaling, or scaling is uniform
  memcpy(normalMatrix, m, 9 * sizeof(float));
}