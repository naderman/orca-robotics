/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp, George Mathews
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef SIMPLE_OGRAYTRACE_H
#define SIMPLE_OGRAYTRACE_H

#include <orcaogmap/ograytracer.h>

namespace orcaogmap {

//!
//! The OgRayTracer stuff's fast and general, but maybe a bit difficult to understand.
//! To make things simpler, this file contains some simple ray tracers that are likely
//! to be commonly used.


//!
//! ILosTracer: interface for classes which determine Line-Of-Sight between two
//!             points in an OgMap.
//!
//! @Author Alex Brooks
//!
class ILosTracer {

public:

    virtual ~ILosTracer() {};

    virtual bool isClearCells( int x0, int y0, int x1, int y1, int &endX, int &endY )=0;
    virtual bool isClearCells( int x0, int y0, int x1, int y1 )=0;
    virtual bool isClearWorld( double x0, double y0, double x1, double y1, double &endX, double &endY )=0;
    virtual bool isClearWorld( double x0, double y0, double x1, double y1 )=0;
};

//!
//! OgLosTracer: useful for deciding whether there is Line-Of-Sight between two
//!              points in an OgMap.
//!
//! @Author Alex Brooks
//!
class OgLosTracer : public ILosTracer {

public:

    OgLosTracer( const orcaogmap::OgMap &ogMap,
                 unsigned char occupiedThreshold=orcaogmap::CELL_UNKNOWN )
        : ogMap_(ogMap),
          safeCellTracer_( ogMap, occupiedThreshold ),
          safeLosRayTracer_( safeCellTracer_ ),
          uncheckedCellTracer_( ogMap, occupiedThreshold ),
          uncheckedLosRayTracer_( uncheckedCellTracer_ )
        {}

    //! Trace from (x0,y0) to (x1,y1).  
    //! Puts the point at which it stopped tracing in (endX,endY).
    //! Returns: 
    //!   true:  we didn't hit an obstacle.
    //!   false: we hit an obstacle or left the map.
    //!
    bool isClearCells( int x0, int y0, int x1, int y1, int &endX, int &endY )
        {
            bool isLineOfSight = safeLosRayTracer_.trace( x0,y0, x1,y1 );
            if ( !isLineOfSight )
            {
                endX = safeCellTracer_.lastEndX();
                endY = safeCellTracer_.lastEndY();
                return isLineOfSight;
            }
            else
            {
                endX = x1;
                endY = y1;
                return isLineOfSight;
            }
        }

    //! Trace from (x0,y0) to (x1,y1).  
    //! Returns: 
    //!   true:  we didn't hit an obstacle.
    //!   false: we hit an obstacle or left the map.
    //!
    bool isClearCells( int x0, int y0, int x1, int y1 )
        {
            if ( ogMap_.cellWithinMap(x0,y0) &&
                 ogMap_.cellWithinMap(x1,y1) )
                return uncheckedLosRayTracer_.trace( x0,y0, x1,y1 );
            else
                return false;
        }

    //! Similar to isClearCells, but specify endpoints in world coords.
    bool isClearWorld( double x0, double y0, double x1, double y1, double &endX, double &endY )
        {
            int x0c,y0c,x1c,y1c,endXc,endYc;
            ogMap_.getCellIndices( x0, y0, x0c, y0c );
            ogMap_.getCellIndices( x1, y1, x1c, y1c );
            bool clear = isClearCells( x0c, y0c, x1c, y1c, endXc, endYc );
            endX = ogMap_.worldXCoord( endXc );
            endY = ogMap_.worldYCoord( endYc );
            return clear;
        }

    //! Similar to isClearCells, but specify endpoints in world coords.
    bool isClearWorld( double x0, double y0, double x1, double y1 )
        {
            int x0c,y0c,x1c,y1c;
            ogMap_.getCellIndices( x0, y0, x0c, y0c );
            ogMap_.getCellIndices( x1, y1, x1c, y1c );
            return isClearCells( x0c, y0c, x1c, y1c );
        }

private:

    const OgMap                    &ogMap_;

    // Use this if we're gonna go off the map, and we want to know when
    SafeLosCellTracer               safeCellTracer_;
    OgRayTracer<SafeLosCellTracer>  safeLosRayTracer_;

    // Use this if we don't care about the exact endpoint, and 
    // we know we're gonna stay on the map
    UncheckedLosCellTracer              uncheckedCellTracer_;
    OgRayTracer<UncheckedLosCellTracer> uncheckedLosRayTracer_;

};




}


#endif
