#include "vector3.h"
#include <iostream>
#include <cmath>
#include <sstream>

using namespace std;

namespace orcaqgui3d {

double Vector3::length() const
{
    return sqrt(x()*x() + y()*y() + z()*z());
}
      
double length(const Vector3 &v)
{
    return sqrt(v.x()*v.x() + v.y()*v.y() + v.z()*v.z());
}

void Vector3::normalise()
{
    double l = length();
    if(l != 0)
    {
        x() /= l;
        y() /= l;
        z() /= l;
    }    
}
      
Vector3 normalise(const Vector3 &v)
{
    Vector3 normalised = v;
    normalised.normalise();
    return normalised;
}

double Vector3::distance(const Vector3 &v) const
{
    return sqrt((v.x()-x())*(v.x()-x()) + (v.y()-y())*(v.y()-y()) + (v.z()-z())*(v.z()-z()));
}  
      
double distance(const Vector3 &v, const Vector3 &w)
{
    return sqrt((v.x()-w.x())*(v.x()-w.x()) + (v.y()-w.y())*(v.y()-w.y()) + (v.z()-w.z())*(v.z()-w.z()));
}

double Vector3::dot(const Vector3 &v) const
{
    return v.x()*x() + v.y()*y() + v.z()*z();
}            
      
      
double dot(const Vector3 &v, const Vector3 &w)
{
    return v.x()*w.x() + v.y()*w.y() + v.z()*w.z();
}

Vector3 Vector3::cross(const Vector3 &v) const
{
    Vector3 u;
      
    u.x() = ((v.y() * z()) - (v.z() * y()));
    u.y() = ((v.z() * x()) - (v.x() * z()));
    u.z() = ((v.x() * y()) - (v.y() * x()));
    return u;    
} 
      
Vector3 cross(const Vector3 &v, const Vector3 &w)
{
    Vector3 u;
      
    u.x() = ((v.y() * w.z()) - (v.z() * w.y()));
    u.y() = ((v.z() * w.x()) - (v.x() * w.z()));
    u.z() = ((v.x() * w.y()) - (v.y() * w.x()));
    return u;
}

//Subtract two vectors
Vector3 operator-(const Vector3 &v, const Vector3 &w)
{
    Vector3 u;
    u.x() = v.x() - w.x();
    u.y() = v.y() - w.y();
    u.z() = v.z() - w.z();
    return u;
}
      
//Return the negative of the vector
Vector3 operator-(const Vector3 &v)
{
    Vector3 u;
    u.x() = -v.x();
    u.y() = -v.y();
    u.z() = -v.z();
    return u;
}
      
//Add two vectors
Vector3 operator+(const Vector3 &v, const Vector3 &w)
{
    Vector3 u;
    u.x() = v.x() + w.x();
    u.y() = v.y() + w.y();
    u.z() = v.z() + w.z();
    return u;
}
      
//Multiplies a vector with a scalar
Vector3 operator*(const Vector3 &v, double a)
{
    Vector3 w;
    w.x() = v.x() * a;
    w.y() = v.y() * a;
    w.z() = v.z() * a;
    return w;
}
      
//Multiplies a vector with a scalar
Vector3 operator*(double a, const Vector3 &v)
{
    Vector3 w;
    w.x() = v.x() * a;
    w.y() = v.y() * a;
    w.z() = v.z() * a;
    return w;
}
      
//Divides a vector through a scalar
Vector3 operator/(const Vector3 &v, double a)
{
    Vector3 w;
    w.x() = v.x() / a;
    w.y() = v.y() / a;
    w.z() = v.z() / a;
    return w;
}


//Gives the angle between two 3D vectors (in radians)
double vectorAngle(const Vector3 &v, const Vector3 &w)
{
    //dot product(v,w) = length(v)*length(w)*cos(angle) ==> angle = acos(dot/(length(v)*length(w))) = acos(dot(norm(v)*norm(w)));
    double cosineOfAngle = dot(normalise(v), normalise(w));
    //for acos, the value has to be between -1.0 and 1.0, but due to numerical imprecisions it sometimes comes outside this range
    if(cosineOfAngle > 1.0) cosineOfAngle = 1.0;
    if(cosineOfAngle < -1.0) cosineOfAngle = -1.0;
    return -acos(cosineOfAngle);
}

//Rotate vector v around arbitrary() ax()is for angle radians
//It can only() rotate around an ax()is through our object, to rotate around another ax()is:
//first translate the object to the ax()is, then use this function, then translate back in the new direction.
Vector3 rotateAroundArbitrary(const Vector3 &v, const Vector3 &axis, double angle)
{
    if((v.x() == 0) && (v.y() == 0) && (v.z() == 0)) return Vector3();
      
    Vector3 w;
    double c, s, t;
      
    Vector3 nAxis = axis;
    nAxis.normalise();
      
    //calculate parameters of the rotation matrix()
    c = cos(angle);
    s = sin(angle);
    t = 1 - c;
      
    //multiply() v with rotation matrix()
    w.x() = (t * nAxis.x() * nAxis.x() +             c) * v.x()
          + (t * nAxis.x() * nAxis.y() + s * nAxis.z()) * v.y()
          + (t * nAxis.x() * nAxis.z() - s * nAxis.y()) * v.z();
             
    w.y() = (t * nAxis.x() * nAxis.y() - s * nAxis.z()) * v.x() 
          + (t * nAxis.y() * nAxis.y() +             c) * v.y() 
          + (t * nAxis.y() * nAxis.z() + s * nAxis.x()) * v.z();
             
    w.z() = (t * nAxis.x() * nAxis.z() + s * nAxis.y()) * v.x() 
          + (t * nAxis.y() * nAxis.z() - s * nAxis.x()) * v.y() 
          + (t * nAxis.z() * nAxis.z() +             c) * v.z();
      
    w.normalise();
    w = w * v.length();
          
    return w;
}

std::string toString( const Vector3 &v )
{
    const double eps = 1e-9;
    stringstream ss;
    ss << "[ "<<((fabs(v.x())<eps)?0:v.x())
       <<", "<<((fabs(v.y())<eps)?0:v.y())
       <<", "<<((fabs(v.z())<eps)?0:v.z())<<" ]";
    return ss.str();
}

}

