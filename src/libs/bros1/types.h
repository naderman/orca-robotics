/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef BROS_1_H
#define BROS_1_H

// Definitions in this file are based on the discussion in the Player mailing list
// taken place in 2003. Specifically, these are C++ encoding of standard
// definitions proposed by Herman Bruyninckx.

namespace bros1
{

// One dimensional ("linear") objects

//! Physical dimenstion, positive [m]. E.g. length.
typedef double Dimension;
//! Discrete dimension, positive. E.g. map width measured in number of cells.
typedef int DiscreteDimension;
//! Rotatation angle with unlimited range [rad].
typedef double RotationAngle; 
//! Orientation angle with range [-pi,+pi] [rad].
typedef double OrientationAngle;
//! Heading angle with with range [-pi/2,+pi/2] [rad].
typedef double HeadingAngle;
//! Angular velocity (z-dir) with unlimited range [rad/s].
typedef double AngularVelocity2d;
//! Angular acceleration (z-dir) with unlimited range [rad/s].
typedef double AngularAcceleration2d;
//! Planar moment (z-dir) [Nm]
typedef double Moment2d;


// Two-dimensional ("planar") objects

//! 2D position in Cartesian coordinate system.
struct CartesianPoint2d
{
    //! Constructor
    CartesianPoint2d( const double& xin=0.0, const double& yin=0.0 ) :
        x(xin), y(yin) {};
    //! x-coordinate [m]
    double x;
    //! y-coordinate [m]
    double y;
};
//! 2D velocity in Cartesian coordinate system.
struct CartesianVelocity2d
{
    //! Constructor
    CartesianVelocity2d( const double& xin=0.0, const double& yin=0.0 ) :
        x(xin), y(yin) {};
    //! x-coordinate [m/s]
    double x;
    //! y-coordinate [m/s]
    double y;
};
//! 2D acceleration in Cartesian coordinate system.
struct CartesianAcceleration2d
{
    //! Constructor
    CartesianAcceleration2d( const double& xin=0.0, const double& yin=0.0 ) :
        x(xin), y(yin) {};
    //! x-coordinate [m/s^2]
    double x;
    //! y-coordinate [m/s^2]
    double y;
};
//! 2D position in polar coordinate system.
struct PolarPoint2d
{
    //! Constructor
    PolarPoint2d( const double& rin=0.0, const double& oin=0.0 ) :
        r(rin), o(oin) {};
    //! Range-coordinate [m]
    double r;
    //! Bearing-coordinate [rad]
    OrientationAngle o;
};
//! 2D velocity in polar coordinate system.
struct PolarVelocity2d
{
    //! Constructor
    PolarVelocity2d( const double& rin=0.0, const double& oin=0.0 ) :
        r(rin), o(oin) {};
    //! Range-coordinate [m/s]
    double r;
    //! Bearing-coordinate [rad/s]
    OrientationAngle o;
};
//! 2D acceleration in polar coordinate system.
struct PolarAcceleration2d
{
    //! Constructor
    PolarAcceleration2d( const double& rin=0.0, const double& oin=0.0 ) :
        r(rin), o(oin) {};
    //! Range-coordinate [m/s^2]
    double r;
    //! Bearing-coordinate [rad/s^2]
    OrientationAngle o;
};
//! 2D vector.
struct Vector2d
{
    //! Constructor
    Vector2d( const double& xin=0.0, const double& yin=0.0 ) :
        x(xin), y(yin) {};
    //! x-coordinate [m]
    double x;
    //! y-coordinate [m]
    double y;
};
//! Planar position.
struct Frame2d
{
    //! Constructor
    Frame2d( const double& px=0.0, const double& py=0.0, const double& oin=0.0 ) :
        p(px,py), o(oin) {};
    //! Convenience constructor
    Frame2d( const CartesianPoint2d& pin, const double& oin=0.0 ) :
        p(pin.x,pin.y), o(oin) {};
    //! Point of origin
    CartesianPoint2d p;
    //! Orientation angle [rad]
    OrientationAngle o;
};
//! Planar velocity.
struct Twist2d
{
    //! Constructor
    Twist2d( const double& vx=0.0, const double& vy=0.0, const double& win=0.0 ) :
        v(vx,vy), w(win) {};
    //! Convenience constructor
    Twist2d( const CartesianVelocity2d& vin, const double& win=0.0 ) :
        v(vin.x,vin.y), w(win) {};
    //! Linear velocity
    CartesianVelocity2d v;
    //! Angular velocity [rad/s]
    AngularVelocity2d w;
};
//! Vector representing a linear force
struct Force2d
{
    //! Constructor
    Force2d( const double& xin=0.0, const double& yin=0.0 ) :
        x(xin), y(yin) {};
    //! x-coordinate [N]
    double x;
    //! y-coordinate [N]
    double y;
};
//! Planar force.
struct Wrench2d
{
    //! Constructor
    Wrench2d( const double& fx=0.0, const double& fy=0.0, const double& min=0.0 ) :
        f(fx,fy), m(min) {};
    //! Convenience constructor
    Wrench2d( const Force2d& fin, const double& min=0.0 ) :
        f(fin.x,fin.y), m(min) {};
    //! Linear force
    Force2d f;
    //! %Moment [N*m]
    Moment2d m;
};
//! Planar continuous dimensions
struct Size2d
{
    //! Constructor
    Size2d( const double& lin=0.0, const double& win=0.0 ) :
        l(lin), w(win) {};
    //! Length [m]
    double l;
    //! Width [m]
    double w;
};
//! Planar discrete dimensions
struct DiscreteSize2d
{
    //! Constructor
    DiscreteSize2d( int lin=0, int win=0 ) :
        l(lin), w(win) {};
    //! Length
    int l;
    //! Width
    int w;
};

// Three-dimensional objects

//! 3D position in Cartesian coordinate system.
struct CartesianPoint3d
{
    //! Constructor
    CartesianPoint3d( const double& xin=0.0, const double& yin=0.0, const double& zin=0.0 ) :
        x(xin), y(yin), z(zin) {};
    //! x-coordinate [m]
    double x;
    //! y-coordinate [m]
    double y;
    //! z-coordinate [m]
    double z;
};
//! 3D vector
struct CartesianVector3d
{
    //! Constructor
    CartesianVector3d( const double& xin=0.0, const double& yin=0.0, const double& zin=0.0 ) :
        x(xin), y(yin), z(zin) {};
    //! x-coordinate [m]
    double x;
    //! y-coordinate [m]
    double y;
    //! z-coordinate [m]
    double z;
};
//! 3D velocity in Cartesian coordinate system.
struct CartesianVelocity3d
{
    //! Constructor
    CartesianVelocity3d( const double& xin=0.0, const double& yin=0.0, const double& zin=0.0 ) :
        x(xin), y(yin), z(zin) {};
    //! x-coordinate [m/s]
    double x;
    //! y-coordinate [m/s]
    double y;
    //! z-coordinate [m/s]
    double z;
};
//! 3D acceleration in Cartesian coordinate system.
struct CartesianAcceleration3d
{
    //! Constructor
    CartesianAcceleration3d( const double& xin=0.0, const double& yin=0.0, const double& zin=0.0 ) :
        x(xin), y(yin), z(zin) {};
    //! x-coordinate [m/s^2]
    double x;
    //! y-coordinate [m/s^2]
    double y;
    //! z-coordinate [m/s^2]
    double z;
};
//! 3D angular velocity.
struct AngularVelocity3d
{
    //! Constructor
    AngularVelocity3d( const double& xin=0.0, const double& yin=0.0, const double& zin=0.0 ) :
        x(xin), y(yin), z(zin) {};
    //! Angular velocity around the x-axis [rad/s]
    double x;
    //! Angular velocity around the y-axis [rad/s]
    double y;
    //! Angular velocity around the z-axis [rad/s]
    double z;
};
//! 3D angular acceleration.
struct AngularAcceleration3d
{
    //! Constructor
    AngularAcceleration3d( const double& xin=0.0, const double& yin=0.0, const double& zin=0.0 ) :
        x(xin), y(yin), z(zin) {};
    //! Angular acceleration around the x-axis [rad/s]
    double x;
    //! Angular acceleration around the y-axis [rad/s]
    double y;
    //! Angular acceleration around the z-axis [rad/s]
    double z;
};
//! Vector representing a linear force
struct Force3d
{
    //! Constructor
    Force3d( const double& xin=0.0, const double& yin=0.0, const double& zin=0.0 ) :
        x(xin), y(yin), z(zin) {};
    //! x-coordinate [N]
    double x;
    //! y-coordinate [N]
    double y;
    //! z-coordinate [N]
    double z;
};
//! %Moment of force.
struct Moment3d
{
    //! Constructor
    Moment3d( const double& xin=0.0, const double& yin=0.0, const double& zin=0.0 ) :
        x(xin), y(yin), z(zin) {};
    //! x-coordinate [N*m]
    double x;
    //! y-coordinate [N*m]
    double y;
    //! z-coordinate [N*m]
    double z;
};
//! Spatial velocity: ordered couple (V,W) of two vectors
struct Twist3d
{
    //! Constructor
    Twist3d( const double& vx=0.0, const double& vy=0.0, const double& vz=0.0,
             const double& wx=0.0, const double& wy=0.0, const double& wz=0.0 ) :
        v(vx,vy,vz), w(wx,wy,wz) {};
    //! Convenience constructor
    Twist3d( const CartesianVelocity3d& vin, const AngularVelocity3d& win ) :
        v(vin.x,vin.y,vin.z), w(win.x,win.y,win.z) {};
    //! Linear velocity
    CartesianVelocity3d v;
    //! Angular velocity
    AngularVelocity3d w;
};
//! Spatial force: ordered couple (F,M) of two vectors
struct Wrench3d
{
    //! Constructor
    Wrench3d( const double& fx=0.0, const double& fy=0.0, const double& fz=0.0,
              const double& mx=0.0, const double& my=0.0, const double& mz=0.0 ) :
        f(fx,fy,fz), m(mx,my,mz) {};
    //! Convenience constructor
    Wrench3d( const Force3d& fin, const Moment3d& min ) :
        f(fin.x,fin.y,fin.z), m(min.x,min.y,min.z) {};
    //! Linear force
    Force3d  f;
    //! %Moment of force
    Moment3d m;
};
//! Orientation defined by angles
struct OrientationE3d
{
    //! Constructor
    OrientationE3d( const double& rin=0.0, const double& pin=0.0, const double& yin=0.0 ) :
        r(rin), p(pin), y(yin) {};
    //! Roll angle [rad]
    OrientationAngle r;
    //! Pitch angle [rad]
    OrientationAngle p;
    //! Yaw angle [rad]
    OrientationAngle y;
};
//! Spatial position
struct Frame3d
{
    //! Constructor
    Frame3d( const double& px=0.0, const double& py=0.0, const double& pz=0.0,
             const double& oroll=0.0, const double& op=0.0, const double& oy=0.0 ) :
        p(px,py,pz), o(oroll,op,oy) {};
    //! Convenience constructor
    Frame3d( const CartesianPoint3d& pin, const OrientationE3d& oin ) :
        p(pin.x,pin.y,pin.z), o(oin.r,oin.p,oin.y) {};
    //! Planar position
    CartesianPoint3d p;
    //! Orientation
    OrientationE3d   o;
};
//! Spatial continuous dimensions.
struct Size3d
{
    //! Constructor
    Size3d( const double& lin=0.0, const double& win=0.0, const double& hin=0.0 ) :
        l(lin), w(win), h(hin) {};
    //! Length [m]
    Dimension l;
    //! Width [m]
    Dimension w;
    //! Height [m]
    Dimension h;
};
//! Spatial discrete dimensions.
struct DiscreteSize3d
{
    //! Constructor
    DiscreteSize3d( int lin=0, int win=0, int hin=0 ) :
        l(lin), w(win), h(hin) {};
    //! Length
    DiscreteDimension l;
    //! Width
    DiscreteDimension w;
    //! Height
    DiscreteDimension h;
};

}; // namespace

#endif
