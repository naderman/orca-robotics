#ifndef ORCAVIEW3D_MATRIX4_H
#define ORCAVIEW3D_MATRIX4_H

#include <string>

namespace orcaqgui3d {

////////////////////////////////////////////////////////////////////////////////
//               [ a b c d ]                                                  //
// Matrix Class  [ e f g h ]                                                  //
//               [ i j k l ]                                                  //
//               [ m n o p ]                                                  //
////////////////////////////////////////////////////////////////////////////////
class Matrix4
{

public: 

    Matrix4() {}

    double operator()(int i,int j) const { return m_[i][j]; }
    double &operator()(int i,int j) { return m_[i][j]; }

private: 

    double m_[4][4];

};

Matrix4 operator*(const Matrix4 &A, const Matrix4 &B);

std::string toString( const Matrix4 &m );

}

#endif
