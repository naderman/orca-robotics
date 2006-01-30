/*
 *  Orca Project: Components for robotics.
 *
 *  Copyright (C) 2004-2006
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef ORCA2_BROS_1_ICE
#define ORCA2_BROS_1_ICE

// Definitions in this file are based on the discussion in the Player mailing list
// taken place in 2003. Specifically, these are Slice encoding of standard
// definitions proposed by XXX.

module orca
{

// One dimensional ("linear") objects
// Note: no type aliasing in slice, e.g. typedef (see Ice manual sec.4.20.2)

// float length
// float width
// float height
// float RotationAngle with unlimited range.
// float OrientationAngle with range [-pi,+pi]
// float HeadingAngle with range [-pi/2,+pi/2]
// float AngularVelocity2d (z-dir) with unlimited range
// float AngularAcceleration2d (z-dir) with unlimited range
// float Moment2d (z-dir)


// Two-dimensional ("planar") objects

//! 2D position in Cartesian coordinate system.
struct CartesianPoint2d
{
    //! x-coordinate [m]
    float x;
    //! y-coordinate [m]
    float y;
};
//! 2D velocity in Cartesian coordinate system.
struct CartesianVelocity2d
{
    //! x-coordinate [m/s]
    float x;
    //! y-coordinate [m/s]
    float y;
};
//! 2D acceleration in Cartesian coordinate system.
struct CartesianAcceleration2d
{
    //! x-coordinate [m/s^2]
    float x;
    //! y-coordinate [m/s^2]
    float y;
};
//! 2D position in polar coordinate system.
struct PolarPoint2d
{
    //! Range-coordinate [m]
    float r;
    //! Bearing-coordinate [rad]
    float o;
};
//! 2D velocity in polar coordinate system.
struct PolarVelocity2d
{
    //! Range-coordinate [m/s]
    float r;
    //! Bearing-coordinate [rad/s]
    float o;
};
//! 2D acceleration in polar coordinate system.
struct PolarAcceleration2d
{
    //! Range-coordinate [m/s^2]
    float r;
    //! Bearing-coordinate [rad/s^2]
    float o;
};
//! 2D vector.
struct Vector2d
{
    //! x-coordinate [m]
    float x;
    //! y-coordinate [m]
    float y;
};
//! Planar position.
struct Frame2d
{
    //! Point of origin
    CartesianPoint2d p;
    //! Orientation angle [rad]
    float            o;
};
//! Planar velocity.
struct Twist2d
{
    //! Linear velocity
    CartesianVelocity2d v;
    //! Angular velocity [rad/s]
    float w;
};
//! Vector representing a linear force
struct Force2d
{
    //! x-coordinate [N]
    float x;
    //! y-coordinate [N]
    float y;
};
//! Planar force.
struct Wrench2d
{
    //! Linear force
    Force2d f;
    //! Moment [N*m]
    float   m;
};
//! Planar dimensions
struct Size2d
{
    //! Length [m]
    float l;
    //! Width [m]
    float w;    
};

// Three-dimensional (standard) objects


//! 3D position in Cartesian coordinate system.
struct CartesianPoint
{
    //! x-coordinate [m]
    float x;
    //! y-coordinate [m]
    float y;
    //! z-coordinate [m]
    float z;
};
//! 3D vector
struct CartesianVector
{
    //! x-coordinate [m]
    float x;
    //! y-coordinate [m]
    float y;
    //! z-coordinate [m]
    float z;
};
//! 3D velocity in Cartesian coordinate system.
struct CartesianVelocity
{
    //! x-coordinate [m/s]
    float x;
    //! y-coordinate [m/s]
    float y;
    //! z-coordinate [m/s]
    float z;
};
//! 3D acceleration in Cartesian coordinate system.
struct CartesianAcceleration
{
    //! x-coordinate [m/s^2]
    float x;
    //! y-coordinate [m/s^2]
    float y;
    //! z-coordinate [m/s^2]
    float z;
};
//! 3D angular velocity.
struct AngularVelocity
{
    //! Angular velocity around the x-axis [rad/s]
    float x;
    //! Angular velocity around the y-axis [rad/s]
    float y;
    //! Angular velocity around the z-axis [rad/s]
    float z;
};
//! 3D angular acceleration.
struct AngularAcceleration
{
    //! Angular acceleration around the x-axis [rad/s]
    float x;
    //! Angular acceleration around the y-axis [rad/s]
    float y;
    //! Angular acceleration around the z-axis [rad/s]
    float z;
};
//! Vector representing a linear force
struct Force
{
    //! x-coordinate [N]
    float x;
    //! y-coordinate [N]
    float y;
    //! z-coordinate [N]
    float z;
};
//! Moment of force.
struct Moment
{
    //! x-coordinate [N*m]
    float x;
    //! y-coordinate [N*m]
    float y;
    //! z-coordinate [N*m]
    float z;
};
//! Spatial velocity: ordered couple (V,W) of two vectors
struct Twist
{
    //! Linear velocity
    CartesianVelocity v;
    //! Angular velocity
    AngularVelocity w;
};
//! Spatial force: ordered couple (F,M) of two vectors
struct Wrench
{
    //! Linear force
    Force  f;
    //! Moment of force
    Moment m;
};
//! Orientation defined by angles
struct OrientationE
{
    //! Roll angle [rad]
    float r;
    //! Pitch angle [rad]
    float p;
    //! Yaw angle [rad]
    float y;
};
//! Spatial position
struct Frame3d
{
    //! Planar position
    CartesianPoint p;
    //! Orientation
    OrientationE   o;
};
//! Spatial dimensions
struct Size3d
{
    //! Length [m]
    float l;
    //! Width [m]
    float w;
    //! Height [m]
    float h;
};

}; // module

#endif
