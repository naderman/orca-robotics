/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

// #include <iostream>
// #include <assert.h>

#include "convertutils.h"

// using namespace std;

namespace orcabros1
{

void convert( const hydrobros1::Size3d& obj3d, hydrobros1::Size2d& obj2d )
{
    obj2d.l = obj3d.l;
    obj2d.w = obj3d.w;
}

void convert( const hydrobros1::CartesianPoint3d& obj3d, hydrobros1::CartesianPoint2d& obj2d )
{
    obj2d.x = obj3d.x;
    obj2d.y = obj3d.y;
}

} // namespace
