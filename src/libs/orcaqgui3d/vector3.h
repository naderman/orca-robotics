#ifndef ORCAQGUI3D_VECTOR3_H
#define ORCAQGUI3D_VECTOR3_H

#include <string>

//
// From http://student.kuleuven.be/~m0216922/CG/3dbasics.html
//

namespace orcaqgui3d {

////////////////////////////////////////////////////////////////////////////////
//               [ x ]                                                        //
// Vector Class  [ y ]                                                        //
//               [ z ]                                                        //
////////////////////////////////////////////////////////////////////////////////
class Vector3
{
public:

    Vector3(double x, double y, double z)
        { v_[0]=x; v_[1]=y; v_[2]=z; }
    Vector3()
        { v_[0]=0; v_[1]=0; v_[2]=0; }
    
    double x() const { return v_[0]; }
    double y() const { return v_[1]; }
    double z() const { return v_[2]; }

    double &x() { return v_[0]; }
    double &y() { return v_[1]; }
    double &z() { return v_[2]; }

    double operator[](int i) const { return v_[i]; }
    double &operator[](int i) { return v_[i]; }
    
    double length() const;
    void normalise();
    double distance(const Vector3 &v) const;
    double dot(const Vector3 &v) const;
    Vector3 cross(const Vector3 &v) const;

private:
    double v_[3];
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
inline std::ostream &operator<<( std::ostream &s, const Vector3 &v )
{ return s << toString(v); }

}

#endif
