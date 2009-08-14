#ifndef HYDROOGMAP_RAYTRACER_H
#define HYDROOGMAP_RAYTRACER_H

#include <hydroportability/sharedlib.h>

namespace hydroogmap {

//!
//! @brief Traces paths through a discrete grid.
//!
//! Templated over the CellTracer so that the compiler can optimise fuck out of the thing.
//! Needs to be supplied with the CellTracer class (see examples in this directory)
//!
//! @author Alex Brooks
//!
template<class CellTracer>
class SOEXPORT RayTracer
{

public: 

    // non-const version
    RayTracer( CellTracer &cellTracer )
        : cellTracer_(cellTracer)
        {}

    //! Trace from cell (x0,y0) to cell (x1,y1), inclusive.
    //! Sets the endpoints which were reached.
    //! CellTracer.traceCell(x,y) is applied to every cell along the trace.
    //! rayTrace is guaranteed to start tracing from the (x0,y0) end.
    //!
    //! If traceCell ever returns false, tracing stops.
    //! If we trace right out to (x1,y1), we call cellTracer.traceSucceeded(x1,y1).
    //!
    //!
    //! Uses Bresenham's Line Algorithm (so it's fast).
    //!
    //! Returns:
    //!   true:  we traced out to (x1,y1)
    //!   false: the trace was short-circuited
    bool trace( int x0, int y0, int x1, int y1 );

private: 

    // These guys return:
    //  true:  we traced out to (x1,y1)
    //  false: the trace was short-circuited
    //
    bool rayTraceOct0( int x0, int y0, int x1, int y1 );
    bool rayTraceOct1( int x0, int y0, int x1, int y1 );
    bool rayTraceOct2( int x0, int y0, int x1, int y1 );
    bool rayTraceOct3( int x0, int y0, int x1, int y1 );
    bool rayTraceOct4( int x0, int y0, int x1, int y1 );
    bool rayTraceOct5( int x0, int y0, int x1, int y1 );
    bool rayTraceOct6( int x0, int y0, int x1, int y1 );
    bool rayTraceOct7( int x0, int y0, int x1, int y1 );
    bool rayTraceOct8( int x0, int y0, int x1, int y1 );

    CellTracer &cellTracer_;
};

template<class CellTracer>
SOEXPORT bool
RayTracer<CellTracer>::trace( int x0, int y0, int x1, int y1 )
{
    /*
    *  Octants are defined as follows:
    *
    *   \2|1/ 
    *   3\|/0
    *   --x--
    *   4/|\7
    *   /5|6\ 
    *
    */
    float slope;
    if ( x0 == x1 )
        slope = 999999;
    else
        slope = (float)(y1-y0) / (float)(x1-x0);

    bool traceWentToEnd;
    if ( slope >= 0.0 && slope <= 1.0 )
    {
        if ( x1 > x0 )
        {
            // octant 0
            traceWentToEnd = rayTraceOct0( x0, y0, x1, y1 );
        }
        else
        {
            // octant 4
            traceWentToEnd = rayTraceOct4( x1, y1, x0, y0 );
        }
    }
    else if ( slope > 1.0 )
    {
        if ( y1 > y0 )
        {
            // octant 1
            traceWentToEnd = rayTraceOct1( x0, y0, x1, y1 );
        }
        else
        {
            // octant 5
            traceWentToEnd = rayTraceOct5( x1, y1, x0, y0 );
        }
    }
    else if ( slope <= -1.0 )
    {
        if ( y1 > y0 )
        {
            // octant 2
            traceWentToEnd = rayTraceOct2( x0, y0, x1, y1 );
        }
        else
        {
            // octant 6
            traceWentToEnd = rayTraceOct6( x1, y1, x0, y0 );
        }
    }
    else // ( slope < 0.0 && slope > -1.0 )
    {
        if ( x1 > x0 )
        {
            // octant 7
            traceWentToEnd = rayTraceOct7( x0, y0, x1, y1 );
        }
        else
        {
            // octant 3
            traceWentToEnd = rayTraceOct3( x1, y1, x0, y0 );
        }
    }

    if ( traceWentToEnd )
    {
        cellTracer_.traceSucceeded( x1, y1 );
    }
    return traceWentToEnd;
}

template<class CellTracer>
SOEXPORT bool
RayTracer<CellTracer>::rayTraceOct0( int x0, int y0, int x1, int y1 )
{
    int y = y0; 
    int dx = 2*(x1-x0);
    int dy = 2*(y1-y0); 
    int dydx = dy-dx; 
    int F = (dy-dx)/2; 

    for (int x=x0; x<=x1; x++) 
    { 
        if ( !cellTracer_.traceCell( x, y ) )
        {
            return false;
        }
        if ( F<0 ) 
        {
            F += dy; 
        }
        else 
        {
            y++; 
            F += dydx;
        } 
    }
    return true;
}

template<class CellTracer>
SOEXPORT bool
RayTracer<CellTracer>::rayTraceOct4( int x0, int y0, int x1, int y1 )
{
    int y = y1; 
    int dx = 2*(x1-x0);
    int dy = 2*(y1-y0); 
    int dydx = dy-dx; 
    int F = (dy-dx)/2; 

    for (int x=x1; x>=x0; x--) 
    { 
        if ( !cellTracer_.traceCell( x, y ) )
        {
            return false;
        }
        if ( F<0 ) 
        {
            F += dy; 
        }
        else 
        {
            y--;
            F += dydx;
        } 
    }
    return true;
}


template<class CellTracer>
SOEXPORT bool
RayTracer<CellTracer>::rayTraceOct1( int x0, int y0, int x1, int y1 )
{
    int x = x0; 
    int dx = 2*(x1-x0);
    int dy = 2*(y1-y0); 
    int dxdy = dx-dy; 
    int F = (dx-dy)/2; 

    for (int y=y0; y<=y1; y++) 
    { 
        if ( !cellTracer_.traceCell( x, y ) )
        {
            return false;
        }
        if ( F<0 ) 
        {
            F += dx; 
        }
        else 
        {
            x++; 
            F += dxdy;
        } 
    }
    return true;
}

template<class CellTracer>
SOEXPORT bool
RayTracer<CellTracer>::rayTraceOct5( int x0, int y0, int x1, int y1 )
{
    int x = x1; 
    int dx = 2*(x1-x0);
    int dy = 2*(y1-y0); 
    int dxdy = dx-dy; 
    int F = (dx-dy)/2; 

    for (int y=y1; y>=y0; y--) 
    { 
        if ( !cellTracer_.traceCell( x, y ) )
        {
            return false;
        }
        if ( F<0 ) 
        {
            F += dx; 
        }
        else 
        {
            x--; 
            F += dxdy;
        } 
    }
    return true;
}

template<class CellTracer>
SOEXPORT bool
RayTracer<CellTracer>::rayTraceOct2( int x0, int y0, int x1, int y1 )
{
    int x = x0; 
    int dx = -2*(x1-x0);
    int dy = 2*(y1-y0); 
    int dxdy = dx-dy; 
    int F = (dx-dy)/2; 

    for (int y=y0; y<=y1; y++) 
    { 
        if ( !cellTracer_.traceCell( x, y ) )
        {
            return false;
        }
        if ( F<0 ) 
        {
            F += dx; 
        }
        else 
        {
            x--; 
            F += dxdy;
        } 
    }
    return true;
}

template<class CellTracer>
SOEXPORT bool
RayTracer<CellTracer>::rayTraceOct6( int x0, int y0, int x1, int y1 )
{
    int x = x1; 
    int dx = -2*(x1-x0);
    int dy = 2*(y1-y0); 
    int dxdy = dx-dy; 
    int F = (dx-dy)/2; 

    for (int y=y1; y>=y0; y--) 
    { 
        if ( !cellTracer_.traceCell( x, y ) )
        {
            return false;
        }
        if ( F<0 ) 
        {
            F += dx; 
        }
        else 
        {
            x++; 
            F += dxdy;
        } 
    }
    return true;
}

template<class CellTracer>
SOEXPORT bool
RayTracer<CellTracer>::rayTraceOct7( int x0, int y0, int x1, int y1 )
{
    int y = y0; 
    int dx = 2*(x1-x0);
    int dy = -2*(y1-y0); 
    int dydx = dy-dx; 
    int F = (dy-dx)/2; 

    for (int x=x0; x<=x1; x++) 
    { 
        if ( !cellTracer_.traceCell( x, y ) )
        {
            return false;
        }
        if ( F<0 ) 
        {
            F += dy; 
        }
        else 
        {
            y--; 
            F += dydx;
        } 
    }
    return true;
}

template<class CellTracer>
SOEXPORT bool
RayTracer<CellTracer>::rayTraceOct3( int x0, int y0, int x1, int y1 )
{
    int y = y1; 
    int dx = 2*(x1-x0);
    int dy = -2*(y1-y0); 
    int dydx = dy-dx; 
    int F = (dy-dx)/2; 

    for (int x=x1; x>=x0; x--) 
    { 
        if ( !cellTracer_.traceCell( x, y ) )
        {
            return false;
        }
        if ( F<0 ) 
        {
            F += dy; 
        }
        else 
        {
            y++; 
            F += dydx;
        } 
    }
    return true;
}

}

#endif
