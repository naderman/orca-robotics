/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

// #include <iostream>
// #include <assert.h>

#include "convertutils.h"

// using namespace std;

namespace orcabros1
{

void convert( const bros1::Size3d& obj3d, bros1::Size2d& obj2d )
{
    obj2d.l = obj3d.l;
    obj2d.w = obj3d.w;
}

void convert( const bros1::CartesianPoint3d& obj3d, bros1::CartesianPoint2d& obj2d )
{
    obj2d.x = obj3d.x;
    obj2d.y = obj3d.y;
}

} // namespace
