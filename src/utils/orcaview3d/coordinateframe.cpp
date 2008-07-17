#include "coordinateframe.h"
#include <iostream>
#include <cmath>
#include <sstream>

using namespace std;

namespace orcaview3d {

CoordinateFrame::CoordinateFrame()
    : fwd_ (1,0,0),
      left_(0,1,0),
      up_  (0,0,1),
      pos_ (0,0,0)
{
}

CoordinateFrame::CoordinateFrame( const Vector3 &pos,
                                  double         roll,
                                  double         pitch,
                                  double         yaw )
    : fwd_ (1,0,0),
      left_(0,1,0),
      up_  (0,0,1),
      pos_(pos)
{
    increaseYawPlanet( yaw );
    increasePitch( pitch );
    increaseRoll( roll );
}

CoordinateFrame::CoordinateFrame( const Matrix4 &m )
{
    set(m);
}

CoordinateFrame::CoordinateFrame( const Vector3 &pos, const Vector3 &fwd, const Vector3 &up )
    : fwd_(fwd),
      left_(-cross(fwd,up)),
      up_(up),
      pos_(pos)
{
}

void 
CoordinateFrame::setOrientation( const Vector3 &fwd, const Vector3 &up )
{
    fwd_ = fwd;
    up_  = up;
    left_ = -cross(fwd,up);
}

void 
CoordinateFrame::translate( const Vector3 &offset )
{
    pos_ = pos_ + offset;
}

void 
CoordinateFrame::rotate( const Vector3 &axis, double angle )
{
    fwd_  = rotateAroundArbitrary( fwd_,  axis, angle );
    up_   = rotateAroundArbitrary( up_,   axis, angle );
    left_ = rotateAroundArbitrary( left_, axis, angle );
}

double
CoordinateFrame::roll() const
{
  // roll is the angle between the plane (world_up, cf_fwd) and the plane (cf_up, cf_fwd)
  // the angle between two planes is the angle between their normals
  // the normals are gotten with cross products
  // the vectorAngle function uses acos and dot product
  double roll = vectorAngle(cross(Vector3(0, 0, 1), fwd_), cross(up_, fwd_));
  if(left_.y() < 0) roll = -roll;
  return roll;
}

double
CoordinateFrame::pitch() const
{
    return( atan2(fwd_.z(),fwd_.x()) );
}

double
CoordinateFrame::yaw() const
{
    return( atan2(fwd_.y(),fwd_.x()) );
}

void 
CoordinateFrame::increaseRoll( double angle )
{
    rotate(fwd_, angle);    
}
void 
CoordinateFrame::increasePitch( double angle )
{
    rotate(left_, angle);
}
void 
CoordinateFrame::increaseYawSpace( double angle )
{
    rotate(up_, -angle);
}
void 
CoordinateFrame::increaseYawPlanet( double angle )
{
    rotate(Vector3(0,0,1), -angle);    
}

Matrix4 
CoordinateFrame::homogeneousMatrix() const
{
    Matrix4 m;

    for ( unsigned int i=0; i < 3; i++ )
    {
        m(i,0) = fwd_[i];
        m(i,1) = left_[i];
        m(i,2) = up_[i];
        m(i,3) = pos_[i];
    }
    m(3,0) = 0;
    m(3,1) = 0;
    m(3,2) = 0;
    m(3,3) = 1;

    return m;
}

void 
CoordinateFrame::multMatrix( const Matrix4 &m )
{
    Matrix4 product = homogeneousMatrix()*m;
    set(product);
}

void 
CoordinateFrame::set( const Matrix4 &m )
{
    fwd_  = Vector3(m(0,0),m(1,0),m(2,0));
    left_ = Vector3(m(0,1),m(1,1),m(2,1));
    up_   = Vector3(m(0,2),m(1,2),m(2,2));
    pos_  = Vector3(m(0,3),m(1,3),m(2,3));
}

//////////////////////////////////////////////////////////////////////

std::string toString( const CoordinateFrame &c )
{
    stringstream ss;
    ss << endl;
    ss<<"fwd:  " << toString(c.fwd()) << endl;
    ss<<"left: " << toString(c.left()) << endl;
    ss<<"up:   " << toString(c.up()) << endl;
    ss<<"pos:  " << toString(c.pos());
    return ss.str();
}

}

