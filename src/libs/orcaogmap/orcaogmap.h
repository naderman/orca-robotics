/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp, George Mathews
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef ORCAOGMAP_CONVERT_H
#define ORCAOGMAP_CONVERT_H

#include <orca/ogmap.h>
#include <hydroogmap/hydroogmap.h>

namespace orcaogmap {
    
    //! Converts an OgMap from a Slice-defined representation to an internal one
    void convert( const orca::OgMapData &input, hydroogmap::OgMap &output );
    
    //! Converts an OgMap from an internal to a Slice-defined representation
    void convert( const hydroogmap::OgMap &input, orca::OgMapData &output, orca::OgMapType mapType=orca::OgMapOccupancy );
    
}
#endif
