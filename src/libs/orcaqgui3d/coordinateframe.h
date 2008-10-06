#ifndef ORCAVIEW3D_COORDINATEFRAME_H
#define ORCAVIEW3D_COORDINATEFRAME_H

#include "vector3.h"
#include "matrix4.h"
#include <iostream>
#include <string>

namespace orcaqgui3d {

//
// @author Alex Brooks
//
// @brief This class represents a coordinate frame in the usual Orca sense:
//        The default coordinate frame has x-y plane is parallel to the ground, z up.
//        A local coordinate frame has the x-axis pointing forwards, y left, and z up.
//
////////////////////////////////////////////////////////////////////////////////
//                   [ fwd.x left.x up.x ]   [ pos.x ]                        //
// Coordinate Frame: [ fwd.y left.y up.y ] + [ pos.y ]                        //
//                   [ fwd.z left.z up.z ]   [ pos.z ]                        //
////////////////////////////////////////////////////////////////////////////////
class CoordinateFrame
{

public: 

    CoordinateFrame();
    CoordinateFrame( const Vector3 &pos,
                     double         roll,
                     double         pitch,
                     double         yaw );
    // Create from homogeneous matrix
    CoordinateFrame( const Matrix4 &m );
    CoordinateFrame( const Vector3 &pos,
                     const Vector3 &fwd,
                     const Vector3 &up );

    const Vector3 &fwd()  const { return fwd_; }
    const Vector3 &left() const { return left_; }
    const Vector3 &up()   const { return up_; }
    const Vector3 &pos()  const { return pos_; }

    void setOrientation( const Vector3 &fwd, const Vector3 &up );

    void translate( const Vector3 &offset );
    void rotate( const Vector3 &axis, double angle );

    // (roll, pitch, yaw) are NOT the native description of the coordinate frame!
    // These shouldn't normally need to be used.
    double roll() const;
    double pitch() const;
    double yaw() const;

    // The two versions of increaseYaw:
    //   - increaseYawSpace rotates around the frame's current z-axis
    //       (imagine yaw-ing a spaceship)
    //   - increaseYawPlanet rotates around the world's z-axis
    //       (imagine yaw-ing around the gravity vector on a planet)
    void increaseRoll( double angle );
    void increasePitch( double angle );
    void increaseYawSpace( double angle );
    void increaseYawPlanet( double angle );

    Matrix4 homogeneousMatrix() const;

    // Transform the coordinate frame by multiplying by another matrix
    void multMatrix( const Matrix4 &m );

    // Assumes the bottom row of m is '0,0,0,1'.
    void set( const Matrix4 &m );

private: 

    Vector3 fwd_;
    Vector3 left_;
    Vector3 up_;
    Vector3 pos_;
};

std::string toString( const CoordinateFrame &c );
inline std::ostream &operator<<( std::ostream &s, const CoordinateFrame &c )
{ return s << toString(c); }

}

#endif
