/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp, George Mathews
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#include "ogmaputils.h"
#include <string>
#include <sstream>
#include <iostream>
#include <cmath>
#include <orcaobj/mathdefs.h>

using namespace std;

namespace orcaogmap {

float OgMap::worldXCoord( int gridX ) const
{
    return float(offset_.p.x + (gridX+0.5)*metresPerCellX_);
}
        
float OgMap::worldYCoord( int gridY ) const
{
    return float(offset_.p.y + (gridY+0.5)*metresPerCellY_);
}

void OgMap::getWorldCoords( int gridX, int gridY, float &worldX, float &worldY ) const
{
    // Todo: handle non-zero orientation.
    assert(offset_.o == 0.0 );

    worldX = worldXCoord( gridX );
    worldY = worldYCoord( gridY );
}

void OgMap::getCellIndices( float worldX, float worldY, int &gridX, int &gridY ) const
{
    gridX = worldToIndexX( worldX );
    gridY = worldToIndexY( worldY );
}


void OgMap::reallocate( int numCellsX, int numCellsY )
{
    numCellsX_ = numCellsX;
    numCellsY_ = numCellsY;
    data_.resize( numCellsX_ * numCellsY_ );
}


void OgMap::fill( unsigned char cellValue )
{
    assert( numCellsX_*numCellsY_ == (int) (data_.size()) );
    memset( &(data_[0]), cellValue, numCellsX_*numCellsY_*sizeof(char) );
}

bool OgMap::tryGridCell( int indX, int indY, unsigned char & cell ) const
{
    float worldX, worldY;
    getWorldCoords( indX, indY, worldX, worldY );
    return tryWorldCell( worldX, worldY, cell);
}

bool OgMap::tryWorldCell( float worldX, float worldY, unsigned char & cell ) const
{
    if ( coordsWithinMap( worldX, worldY ) == true )
    {
        cell = worldCell( worldX, worldY );
        return true;
    }
    return false;
}

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
        ss<<"     | ";
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
    assert( master.metresPerCellX() == update.metresPerCellX() );
    assert( master.metresPerCellY() == update.metresPerCellY() );
    assert( master.offset().o == 0.0 );
    assert( update.offset().o == 0.0 );
    if ( master.numCellsX() == 0 && master.numCellsY() == 0 )
    {
        master = update;
        return;
    }

    //determine the number of cells the lower right corner of the tile is offset (in the -ve x and y directions) w.r.t the current map
    int offsetX = (int) floor( (master.offset().p.x - update.offset().p.x) / master.metresPerCellX() + 0.5 );
    int offsetY = (int) floor( (master.offset().p.y - update.offset().p.y) / master.metresPerCellY() + 0.5 );

    //extend in NegX ?
    int dNegX = 0;
    if( offsetX > 0 )
    {
        dNegX = offsetX;

        // setup new offset_
        master.offset().p.x = master.offset().p.x - dNegX * master.metresPerCellX();
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
        master.offset().p.y = master.offset().p.y - dNegY * master.metresPerCellY();
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
    assert( master.metresPerCellX() == slave.metresPerCellX() );
    assert( master.metresPerCellY() == slave.metresPerCellY() );    
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

std::string toString(const CartesianPoint2d &p)
{
    std::stringstream ss;
    ss << "["<<p.x<<","<<p.y<<"]";
    return ss.str();
}
std::string toString(const Frame2d &f)
{
    std::stringstream ss;
    ss << "["<<f.p.x<<","<<f.p.y<<","<<f.o*180.0/M_PI<<"deg]";
    return ss.str();
}

}

std::ostream &operator<<( std::ostream &s, const orcaogmap::OgMap &o )
{
    s << " OgMap: \n"
      << "\toffset:        [" << o.offset().p.x << ", " << o.offset().p.y << ", " << o.offset().o*180.0/M_PI << "]\n"
      << "\tnumCells:      [" << o.numCellsX() << ", " << o.numCellsY() << "]\n"
      << "\tmetresPerCell: [" << o.metresPerCellX() << ", " << o.metresPerCellY() << "]\n";

    return s;
}

