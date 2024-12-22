#ifndef _MATH_OPERATION_H_
#define _MATH_OPERATION_H_

// variables
extern float M[16];

// basic operations
void normalize(float* v);
void multiplyMatrices(int m, int p, int n, const float * A, const float * B, float * C);
void crossProduct(const float* a, const float* b, float* result);

// compute
void computeTangent(const float u, const float* M, const float* C, float* T);
void computeBinormal(const float* tangent, const float* prevNormal, float* binormal);
void computeNormal(const float* tangent, float* normal, float* binormal);
void computeNormalMatrix(const float* modelViewMatrix, float* normalMatrix);

#endif