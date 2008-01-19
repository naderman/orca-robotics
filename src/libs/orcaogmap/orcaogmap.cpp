/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp, George Mathews
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#include "orcaogmap.h"


namespace orcaogmap {

void 
convert( const orca::OgMapData & input, hydroogmap::OgMap & output )
{
    assert( input.numCellsX*input.numCellsY == (int) (input.data.size()) );

    output.reallocate( input.numCellsX, input.numCellsY );
    memcpy( output.data(), &input.data[0], input.data.size()*sizeof(unsigned char) );

    output.offset().p.x = input.offset.p.x;
    output.offset().p.y = input.offset.p.y;
    output.offset().o = input.offset.o;
    output.setMetresPerCellX( input.metresPerCellX );
    output.setMetresPerCellY( input.metresPerCellY );
}

void 
convert( const hydroogmap::OgMap &input, orca::OgMapData &output, orca::OgMapType mapType )
{
    output.numCellsX = input.numCellsX();
    output.numCellsY = input.numCellsY();
    output.data.resize( output.numCellsX*output.numCellsY );
    memcpy( &(output.data[0]), input.data(), output.data.size() );

    output.offset.p.x = input.offset().p.x;
    output.offset.p.y = input.offset().p.y;
    output.offset.o   = input.offset().o;
    output.metresPerCellX = input.metresPerCellX();
    output.metresPerCellY = input.metresPerCellY();
    output.mapType = mapType;
}

} // namespace
