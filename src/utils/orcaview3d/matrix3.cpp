#if 0
#include "matrix3.h"
#include <iostream>

using namespace std;

namespace orcaview3d {

Matrix3::Matrix3(double a, double b, double c, double d, double e, double f, double g, double h, double i)
{
  this->a = a;
  this->b = b;
  this->c = c;
  this->d = d;
  this->e = e;
  this->f = f;
  this->g = g;
  this->h = h;
  this->i = i;
}
  
Matrix3::Matrix3()
{
  a = 0.0;
  b = 0.0;
  c = 0.0;
  d = 0.0;
  e = 0.0;
  f = 0.0;
  g = 0.0;
  h = 0.0;
  i = 0.0;
}

////////////////////////////////////////////////////////////////////////////////
//Transpose:                                                                  //
//                                                                            //
// [ a b c ]T    [ a d g ]                                                    //
// [ d e f ]  =  [ b e h ]                                                    //
// [ g h i ]     [ c f i ]                                                    //
////////////////////////////////////////////////////////////////////////////////
void Matrix3::transpose()
{
  b += d; d = b - d; b -= d; //swap b and d
  c += g; g = c - g; c -= g; //swap c and g
  f += h; h = f - h; f -= h; //swap f and h
}  

Matrix3 transpose(const Matrix3& A)
{
  Matrix3 result = A;
  result.transpose();
  return result;
} 

double Matrix3::determinant() const
{
  double det = a * e * i 
             + b * f * g 
             + d * h * c 
             - g * e * c 
             - d * b * i 
             - h * f * a;
  return det;
}

double determinant(const Matrix3& A)
{
  return A.determinant();
}

//Inverse of a 3x3 matrix
void Matrix3::invert()
{
  /*
  the inverse is the adjoint divided through the determinant
  find the matrix of minors (minor = determinant of 2x2 matrix of the 2 rows/colums current element is NOT in)
  turn them in cofactors (= change some of the signs)
  find the adjoint by transposing the matrix of cofactors
  divide this through the determinant to get the inverse
  */
  
  double det = determinant();
  Matrix3 B;

  //included in these calculations: minor, cofactor (changed signs), transpose (by the order of "="), division through determinant
  B.a = ( e * i - h * f) / det;
  B.b = (-b * i + h * c) / det;
  B.c = ( b * f - e * c) / det;
  B.d = (-d * i + g * f) / det;
  B.e = ( a * i - g * c) / det;
  B.f = (-a * f + d * c) / det;
  B.g = ( d * h - g * e) / det;
  B.h = (-a * h + g * b) / det;
  B.i = ( a * e - d * b) / det;
  
  *this = B;
}

Matrix3 inverse(const Matrix3& A)
{
  Matrix3 result = A;
  result.invert();
  return result;
} 

//Add two matrices
Matrix3 operator+(const Matrix3& A, const Matrix3& B)
{
  Matrix3 result = A;
  result.a += B.a;
  result.b += B.b;
  result.c += B.c;
  result.d += B.d;
  result.e += B.e;
  result.f += B.f;
  result.g += B.g;
  result.h += B.h;
  result.i += B.i;
  return result;
}

//Subtract two matrices
Matrix3 operator-(const Matrix3& A, const Matrix3& B)
{
  Matrix3 result = A;
  result.a -= B.a;
  result.b -= B.b;
  result.c -= B.c;
  result.d -= B.d;
  result.e -= B.e;
  result.f -= B.f;
  result.g -= B.g;
  result.h -= B.h;
  result.i -= B.i;
  return result;
}

//Multiply a matrix with a scalar
Matrix3 operator*(const Matrix3& A, double a)
{
  Matrix3 result = A;
  result.a *= a;
  result.b *= a;
  result.c *= a;
  result.d *= a;
  result.e *= a;
  result.f *= a;
  result.g *= a;
  result.h *= a;
  result.i *= a;
  return result;
}

//Multiply a matrix with a scalar
Matrix3 operator*(double a, const Matrix3& A)
{
  Matrix3 result = A;
  result.a *= a;
  result.b *= a;
  result.c *= a;
  result.d *= a;
  result.e *= a;
  result.f *= a;
  result.g *= a;
  result.h *= a;
  result.i *= a;
  return result;
}

//Divide a matrix through a scalar
Matrix3 operator/(const Matrix3& A, double a)

{
  Matrix3 result = A;
  result.a /= a;
  result.b /= a;
  result.c /= a;
  result.d /= a;
  result.e /= a;
  result.f /= a;
  result.g /= a;
  result.h /= a;
  result.i /= a;
  return result;
}

//Multiply a matrix with a column vector, resulting in a column vector
Vector3 operator*(const Matrix3& A, const Vector3& v)
{
  Vector3 w;
  w.x = A.a * v.x + A.b * v.y + A.c * v.z;
  w.y = A.d * v.x + A.e * v.y + A.f * v.z;
  w.z = A.g * v.x + A.h * v.y + A.i * v.z;
  return w;
}  

//Multiply a vector with a row matrix, resulting in a row vector
Vector3 operator*(const Vector3& v, const Matrix3& A)
{
  Vector3 w;
  w.x = A.a * v.x + A.d * v.y + A.g * v.z;
  w.y = A.b * v.x + A.e * v.y + A.h * v.z;
  w.z = A.c * v.x + A.f * v.y + A.i * v.z;
  return w;
}

//Multiply a 3x3 matrix with a 3x3 matrix
Matrix3 operator*(const Matrix3& A, const Matrix3& B)
{
  Matrix3 C;
  C.a = A.a * B.a + A.b * B.d + A.c * B.g;
  C.b = A.a * B.b + A.b * B.e + A.c * B.h;
  C.c = A.a * B.c + A.b * B.h + A.c * B.i;
  C.d = A.d * B.a + A.e * B.d + A.f * B.g;
  C.e = A.d * B.b + A.e * B.e + A.f * B.h;
  C.f = A.d * B.c + A.e * B.h + A.f * B.i;
  C.g = A.g * B.a + A.h * B.d + A.i * B.g;
  C.h = A.g * B.b + A.h * B.e + A.i * B.h;
  C.i = A.g * B.c + A.h * B.h + A.i * B.i;
  return C;
}

}

#endif
