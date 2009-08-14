/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp, George Mathews
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include "ogmap.h"
#include <iostream>
#include <sstream>
#include <gbxutilacfr/mathdefs.h>

using namespace std;

namespace hydroogmap {

std::string toText( const OgMap &map )
{
    std::stringstream ss;
    int szX=map.numCellsX();
    int szY=map.numCellsY();
    int x, y;
    int step = 1 + map.numCellsX() / 200;

    ss<<endl;
    ss<<"      ";
    for( x=0; x<szX; x+=step )
    {
        ss<<"-";
    }
    ss<<"-"<<endl;

    for( y=szY-1; y>=0; y-=step)
    {
        ss<<"     |";
        for( x=0; x<szX; x+=step)
        {
            ss<<displayOgmapCell( map.gridCell(x,y) );
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

void fuse( OgMap &master, const OgMap &update, unsigned char newCells )
{
    // Check assumptions
    assert( master.metresPerCell() == update.metresPerCell() );
    assert( master.offset().o == 0.0 );
    assert( update.offset().o == 0.0 );
    if ( master.numCellsX() == 0 && master.numCellsY() == 0 )
    {
        master = update;
        return;
    }

    //determine the number of cells the lower right corner of the tile is offset (in the -ve x and y directions) w.r.t the current map
    int offsetX = (int) floor( (master.offset().p.x - update.offset().p.x) / master.metresPerCell() + 0.5 );
    int offsetY = (int) floor( (master.offset().p.y - update.offset().p.y) / master.metresPerCell() + 0.5 );

    //extend in NegX ?
    int dNegX = 0;
    if( offsetX > 0 )
    {
        dNegX = offsetX;

        // setup new offset_
        master.offset().p.x = master.offset().p.x - dNegX * master.metresPerCell();
        //cout<<"  extending in neg X dir: offsetX="<<offsetX<<", dNegX="<<dNegX<<", offsetX_"<<offsetX_<<endl;
    }

    //extend in PosX
    int dPosX = 0;
    if( update.numCellsX() - offsetX > master.numCellsX() )
    {
        dPosX = update.numCellsX() - offsetX - master.numCellsX();
        //cout<<"  extending in pos X dir: offsetX="<<offsetX<<", dPosX="<<dPosX<<endl;
    }

    //extend in NegY ?
    int dNegY = 0;
    if( offsetY > 0 )
    {
        dNegY = offsetY;

        // setup new offset_
        master.offset().p.y = master.offset().p.y - dNegY * master.metresPerCell();
        //cout<<"  extending in neg Y dir: offsetY="<<offsetY<<", dNegY="<<dNegY<<", offsetY_"<<offsetY_<<endl;
    }

    //extend in PosY
    int dPosY = 0;
    if( update.numCellsY() - offsetY > master.numCellsY() )
    {
        dPosY = update.numCellsY() - offsetY - master.numCellsY();
        //cout<<"  extending in pos Y dir: offsetY="<<offsetY<<", dPosY="<<dPosY<<endl;
    }

    // Save old map info
    int oldSizeX = master.numCellsX();
    int oldSizeY = master.numCellsY();

    int oldXMin = dNegX;
    int oldXMax = dNegX + oldSizeX;

    int oldYMin = dNegY;
    int oldYMax = dNegY + oldSizeY;

//     unsigned int oldLength = oldSizeX * oldSizeY;
//     unsigned char* oldArray = new unsigned char[oldLength];
//     memcpy( oldArray, master.data_, oldLength ); //oldlength is the length in bytes!

    // Keep a copy of the original
    OgMap oldMap = master;

    // new map size
    int newSizeX = oldSizeX + dNegX + dPosX;
    int newSizeY = oldSizeY + dNegY + dPosY;

    //cout <<"new size: X "<<sizeX_<<", Y "<<sizeY_<<endl;

    //create new map data_ array
    master.reallocate( newSizeX, newSizeY );

//     unsigned char * newArray = master.data_;
//     const unsigned char * updateArray = update.data_;

    // update info
    int updateXMin = MAX(0,-offsetX);
    int updateXMax = updateXMin + update.numCellsX();

    int updateYMin = MAX(0,-offsetY);
    int updateYMax = updateYMin + update.numCellsY();

    //cout << "OGMap::update: oldXMin "<<oldXMin<<", oldXMax "<<oldXMax<<", oldYMin "<<oldYMin<<", oldYMax"<<oldYMax<<endl;
    //cout << "OGMap::update: updateXMin "<<updateXMin<<", updateXMax "<<updateXMax<<", updateYMin "<<updateYMin<<", updateYMax"<<updateYMax<<endl;

    int newIndex=0;
    int oldIndex=0;
    int updateIndex=0;

    //cout<<"inserting new cells for size: "<<newSizeX<<","<<newSizeY<<endl;

    for( int y=0; y<newSizeY; y++ )
    {
        for( int x=0; x<newSizeX; x++ )
        {
            //cout<<"(x,y)=("<<x<<","<<y<<")";

            //start with default value
//            newArray[newIndex] = newcells;
            master.data()[newIndex] = newCells;
            //check if inside old map
            if( (x >= oldXMin) && (x < oldXMax) && (y >= oldYMin) && (y < oldYMax) )
            {
                //cout << " is in old";
//                newArray[newIndex] = oldArray[oldIndex++];
                master.data()[newIndex] = oldMap.data()[oldIndex++];
            }

            //check if inside update map (doesn't matter if in both)
            if( (x >= updateXMin) && (x < updateXMax) && (y >= updateYMin) && (y < updateYMax) )
            {
                //cout << " is in update";
                master.data()[newIndex] = update.data()[updateIndex++];
            }
            //cout<<endl;
            newIndex++;
        }
    }
}

void overlay( OgMap &master, const OgMap &slave )
{
    // Check assumptions
    assert( master.metresPerCell() == slave.metresPerCell() );
    assert( master.numCellsX() == slave.numCellsX() );
    assert( master.numCellsY() == slave.numCellsY() );
    assert( master.offset().p.x == slave.offset().p.x );
    assert( master.offset().p.y == slave.offset().p.y );
    assert( master.offset().o == slave.offset().o );    

    for (int x=0; x<master.numCellsX(); x++)
    {
        for (int y=0; y<master.numCellsY(); y++)
        {
            int valueA = (int)master.gridCell(x,y);
            int valueB = (int)slave.gridCell(x,y);
            master.gridCell(x,y) = (unsigned char)(valueA | valueB);
        }
    }
}

}

