/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <assert.h>
// #include <IceUtil/Time.h>
// #include <orcaice/orcaice.h>

#include "convertutils.h"

using namespace std;

namespace orcabros1
{

// C++ to Slice Conversion Functions
void convert( const hydrobros1::Size2d& cobj, orca::Size2d& iceobj )
{
    iceobj.l = cobj.l;
    iceobj.w = cobj.w;
}
void convert( const hydrobros1::CartesianPoint2d& cobj, orca::CartesianPoint2d& iceobj )
{
    iceobj.x = cobj.x;
    iceobj.y = cobj.y;
}
void convert( const hydrobros1::CartesianPoint3d& cobj, orca::CartesianPoint& iceobj )
{
    iceobj.x = cobj.x;
    iceobj.y = cobj.y;
    iceobj.z = cobj.z;
}
void convert( const hydrobros1::CartesianPoint3d& cobj, orca::CartesianPoint2d& iceobj )
{
    iceobj.x = cobj.x;
    iceobj.y = cobj.y;
}
void convert( const hydrobros1::Frame3d& cobj, orca::Frame3d& iceobj )
{
    iceobj.p.x = cobj.p.x;
    iceobj.p.y = cobj.p.y;
    iceobj.p.z = cobj.p.z;
    iceobj.o.r = cobj.o.r;
    iceobj.o.p = cobj.o.p;
    iceobj.o.y = cobj.o.y;
}


//  Slice to C++ Conversion Functions
void convert( const orca::Size2d& iceobj, hydrobros1::Size2d& cobj )
{
    cobj.l = iceobj.l;
    cobj.w = iceobj.w;
}
void convert( const orca::CartesianPoint2d& iceobj, hydrobros1::CartesianPoint2d& cobj )
{
    cobj.x = iceobj.x;
    cobj.y = iceobj.y;
}
void convert( const orca::CartesianPoint& iceobj, hydrobros1::CartesianPoint3d& cobj )
{
    cobj.x = iceobj.x;
    cobj.y = iceobj.y;
    cobj.z = iceobj.z;
}
void convert( const orca::CartesianPoint& iceobj, hydrobros1::CartesianPoint2d& cobj )
{
    cobj.x = iceobj.x;
    cobj.y = iceobj.y;
}
void convert( const orca::Frame3d& iceobj, hydrobros1::Frame3d& cobj )
{
    cobj.p.x = iceobj.p.x;
    cobj.p.y = iceobj.p.y;
    cobj.p.z = iceobj.p.z;
    cobj.o.r = iceobj.o.r;
    cobj.o.p = iceobj.o.p;
    cobj.o.y = iceobj.o.y;
}

} // namespace
