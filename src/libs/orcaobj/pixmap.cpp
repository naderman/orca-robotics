/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <sstream>
#include <cmath>
#include "pixmap.h"
#include "datetime.h"

using namespace std;

namespace orcaobj
{

std::string toString( const orca::PixMapData& obj )
{
    std::ostringstream s;
    s << toString(obj.timeStamp)
      << " PixMap: \n"
      << "offset:        [" << obj.offset.p.x << ", " << obj.offset.p.y << ", " << obj.offset.o*180.0/M_PI << "]\n"
      << "numCells:      [" << obj.numCellsX << ", " << obj.numCellsY << "]\n"
      << "metresPerCell: [" << obj.metresPerCell << "]\n";

    return s.str();
}

} // namespace
