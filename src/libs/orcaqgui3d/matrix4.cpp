#include "matrix4.h"
#include <iostream>
#include <sstream>
#include <cmath>

using namespace std;

namespace orcaqgui3d {

Matrix4 operator*(const Matrix4 &A, const Matrix4 &B)
{
    Matrix4 C;
    
    for ( unsigned int i=0; i < 4; i++ )
    {
        for ( unsigned int j=0; j < 4; j++ )
        {
            C(i,j) = 0;
            for ( unsigned int k=0; k < 4; k++ )
                C(i,j) += A(i,k)*B(k,j);
        }
    }

    return C;
}

std::string toString( const Matrix4 &m )
{
    const double eps = 1e-9;

    stringstream ss;
    ss << endl;
    for ( unsigned int i=0; i < 4; i++ )
    {
        ss << "[ ";
        for ( unsigned int j=0; j < 4; j++ )
        {
            ss << ((fabs(m(i,j))<eps)?0:m(i,j)) << " ";
        }
        if ( i == 3 )
            ss << "];";
        else
            ss << "; ...\n";
    }
    return ss.str();
}


}

