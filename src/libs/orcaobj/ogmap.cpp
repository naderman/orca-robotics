/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <sstream>
#include <cmath>
#include "ogmap.h"
#include "datetime.h"

using namespace std;

namespace orcaobj
{

// Utility function for toVerboseString:
// display a map cell:
char displayOgmapCell(unsigned char cell)
{
    if( (int)cell < 96 )
    {
        return ' ';
    }
    else if( (int)cell > 160 )
    {
        return '#';
    }
    //else
    return '-';
}

std::string toString( const orca::OgMapData& obj )
{
    std::ostringstream s;
    s << toString(obj.timeStamp)
      << " OgMap: \n"
      << "offset:        [" << obj.offset.p.x << ", " << obj.offset.p.y << ", " << obj.offset.o*180.0/M_PI << "]\n"
      << "numCells:      [" << obj.numCellsX << ", " << obj.numCellsY << "]\n"
      << "metresPerCell: [" << obj.metresPerCellX << ", " << obj.metresPerCellY << "]\n";

    return s.str();
}

std::string toVerboseString( const orca::OgMapData& obj )
{
    std::stringstream ss;
    int szX=obj.numCellsX;
    int szY=obj.numCellsY;
    int x, y;
    int step = 1 + obj.numCellsX / 200;

    ss<<endl;
    ss<<"      ";
    for( x=0; x<szX; x+=step )
    {
        ss<<"-";
    }
    ss<<"-"<<endl;

    for( y=szY-1; y>=0; y-=step)
    {
        ss<<"     | ";
        for( x=0; x<szX; x+=step)
        {
            ss<<displayOgmapCell( obj.data[ gridCellIndex(obj,x,y) ] );
        }
        ss<<"|"<<endl;
    }

    ss<<"  y   ";
    for( x=0; x<szX; x+=step )
    {
        ss<<"-";
    }
    ss<<"-"<<endl;

    ss<<"  |"<<endl;
    ss<<"  |____ x"<<endl;
    return ss.str();
}

} // namespace
