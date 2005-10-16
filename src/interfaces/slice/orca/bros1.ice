/*
 *  Orca-Base: Components for robotics.
 *
 *  Copyright (C) 2004
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

struct CartesianPoint2d
{
    float x;
    float y;
};
struct CartesianVelocity2d
{
    float x;
    float y;
};
struct CartesianAcceleration2d
{
    float x;
    float y;
};
struct PolarPoint2d
{
    float r;
    float a;
};
struct PolarVelocity2d
{
    float r;
    float a;
};
struct PolarAcceleration2d
{
    float r;
    float a;
};
struct Vector2d
{
    float x;
    float y;
};
// Frame
struct Frame2d
{
    CartesianPoint2d point;
    float a;    // OrientationAngle
};
struct Twist2d
{
    CartesianVelocity2d velocity;
    float w; // degenerated AngularVelocity2d
};
// Vector representing a linear force
struct Force2d
{
    float x;
    float y;
};
struct Wrench2d
{
  Force2d f;
  float m; // degenerated Moment2d
};
struct Size2d
{
    float l; // length
    float w; // width
};

// Three-dimensional (standard) objects
struct CartesianPoint
{
    float x;
    float y;
};
struct CartesianVector
{
    float x;
    float y;
    float z;
};
struct CartesianVelocity
{
    float x;
    float y;
    float z;
};
struct CartesianAcceleration
{
    float x;
    float y;
    float z;
};
struct AngularVelocity
{
    float x;
    float y;
    float z;
};
struct AngularAcceleration
{
    float x;
    float y;
    float z;
};
// Vector representing a linear force
struct Force
{
  float x;
  float y;
  float z;
};
// Moment: Vector representing a force moment
struct Moment
{
  float x;
  float y;
  float z;
};
// Twist: ordered couple (V,W) of two Vectors
struct Twist
{
    CartesianVelocity v;
    AngularVelocity w;
};
// Wrench: ordered couple (F,M) of two Vectors
struct Wrench
{
  Force f;
  Moment m;
};

}; // module

#endif
