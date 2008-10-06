#if 0
#ifndef ORCAQGUI3D_MATRIX3_H
#define ORCAQGUI3D_MATRIX3_H

#include "vector3.h"

//
// From http://student.kuleuven.be/~m0216922/CG/3dbasics.html
//

namespace orcaview3d {

////////////////////////////////////////////////////////////////////////////////
//               [ a b c ]                                                    //
// Matrix Class  [ d e f ]                                                    //
//               [ g h i ]                                                    //
////////////////////////////////////////////////////////////////////////////////

class Matrix3
{
    public:
        
    double a; double b; double c;
    double d; double e; double f;
    double g; double h; double i;
    
    Matrix3(double a, double b, double c, double d, double e, double f, double g, double h, double i);
    Matrix3();
    
    void transpose();
    double determinant() const;
    void invert();
};
      
Matrix3 transpose(const Matrix3 &A);
double determinant(const Matrix3 &A);
Matrix3 inverse(const Matrix3 &A);
Matrix3 operator+(const Matrix3 &A, const Matrix3 &B);
Matrix3 operator-(const Matrix3 &A, const Matrix3 &B);
Matrix3 operator*(const Matrix3 &A, double a);
Matrix3 operator*(double a, const Matrix3 &A);
Matrix3 operator/(const Matrix3 &A, double a);
Vector3 operator*(const Matrix3 &A, const Vector3 &v);
Vector3 operator*(const Vector3 &v, const Matrix3 &A);
Matrix3 operator*(const Matrix3 &A, const Matrix3 &B);

}

#endif
#endif
