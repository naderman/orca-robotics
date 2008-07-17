#ifndef ORCAQGUI3D_VECTOR3_H
#define ORCAQGUI3D_VECTOR3_H

#include <string>

//
// From http://student.kuleuven.be/~m0216922/CG/3dbasics.html
//

namespace orcaview3d {

////////////////////////////////////////////////////////////////////////////////
//               [ x ]                                                        //
// Vector Class  [ y ]                                                        //
//               [ z ]                                                        //
////////////////////////////////////////////////////////////////////////////////
class Vector3
{
public:

    double x;
    double y;
    double z;
    
    Vector3(double xp, double yp, double zp)
        : x(xp),y(yp),z(zp) {}
    Vector3()
        : x(0),y(0),z(0) {}
    
    double length() const;
    void normalise();
    double distance(const Vector3 &v) const;
    double dot(const Vector3 &v) const;
    Vector3 cross(const Vector3 &v) const;
};

double length(const Vector3 &v);
Vector3 normalise(const Vector3 &v);
double distance(const Vector3 &v, const Vector3 &w);       
double dot(const Vector3 &v, const Vector3 &w);
Vector3 cross(const Vector3 &v, const Vector3 &w);
Vector3 operator-(const Vector3 &v, const Vector3 &w);
Vector3 operator-(const Vector3 &v);
Vector3 operator+(const Vector3 &v, const Vector3 &w);
Vector3 operator*(const Vector3 &v, double a);
Vector3 operator*(double a, const Vector3 &v);
Vector3 operator/(const Vector3 &v, double a);

// Gives the angle between two 3D vectors (in radians)
double vectorAngle(const Vector3 &v, const Vector3 &w);

// Rotate vector v around arbitrary axis for angle radians.  It can only
// rotate around an axis through our object, to rotate around another
// axis: first translate the object to the axis, then use this
// function, then translate back in the new direction.
Vector3 rotateAroundArbitrary(const Vector3 &v, const Vector3 &axis, double angle);

std::string toString( const Vector3 &v );

}

#endif
