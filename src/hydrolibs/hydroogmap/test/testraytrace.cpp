/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp, George Mathews
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
// Ensure that assertions are compiled in
#undef NDEBUG

#include <iostream>
#include <sstream>
#include <cstdlib>
#include <hydroogmap/hydroogmap.h>
#include <hydroogmap/mapraytracer.h>
#include <hydroogmap/oglostracer.h>
#include <hydroutil/diff.h>

using namespace std;

class MarkCellTracer
{
public:
    MarkCellTracer(hydroogmap::OgMap &theMap)
        : theMap_(theMap)
        {}
    
    // Apply this function to every cell along the trace
    inline bool traceCell(int x, int y)
        {
            theMap_.gridCell(x,y) = 254;
            return true;
        }

    //  Don't care about the trace succeeding.
    void traceSucceeded( int x, int y ) {};
    //  Don't care about the trace leaving the map.
    void traceLeftMap( int x, int y ) {};

    hydroogmap::OgMap &theMap_;
};

typedef hydroogmap::MapRayTracer<hydroogmap::OgMap,MarkCellTracer> RayTracer;

void testBasic( std::ostream &s )
{
    hydroogmap::OgMap theMap( 20, 20,
                              0, 0, 0,
                              0.1,
                              0 );

    MarkCellTracer t(theMap);
    RayTracer rayTracer( theMap, t );

    rayTracer.traceCells( 10, 10, 10+9, 10+5 );
    rayTracer.traceCells( 10, 10, 10+5, 10+9 );
    rayTracer.traceCells( 10, 10, 10-5, 10+9 );
    rayTracer.traceCells( 10, 10, 10-9, 10+5 );
    rayTracer.traceCells( 10, 10, 10-9, 10-5 );
    rayTracer.traceCells( 10, 10, 10-5, 10-9 );
    rayTracer.traceCells( 10, 10, 10+5, 10-9 );
    rayTracer.traceCells( 10, 10, 10+9, 10-5 );

    rayTracer.traceCells( 0, 10, 19, 9 );

    s << hydroogmap::toText( theMap ) << endl;
}

void testBorders( std::ostream &s )
{
    hydroogmap::OgMap theMap( 20, 20,
                             0, 0, 0,
                             0.1,
                             0 );

    MarkCellTracer t(theMap);
    RayTracer rayTracer( theMap, t );

    rayTracer.traceCells( 10, 10, 10+9, 10+0 );
    s << hydroogmap::toText( theMap ) << endl;
    rayTracer.traceCells( 10, 10, 10+9, 10+9 );
    s << hydroogmap::toText( theMap ) << endl;
    rayTracer.traceCells( 10, 10, 10+0, 10+9 );
    s << hydroogmap::toText( theMap ) << endl;
    rayTracer.traceCells( 10, 10, 10-9, 10+9 );
    s << hydroogmap::toText( theMap ) << endl;
    rayTracer.traceCells( 10, 10, 10-9, 10-0 );
    s << hydroogmap::toText( theMap ) << endl;
    rayTracer.traceCells( 10, 10, 10-9, 10-9 );
    s << hydroogmap::toText( theMap ) << endl;
    rayTracer.traceCells( 10, 10, 10+0, 10-9 );
    s << hydroogmap::toText( theMap ) << endl;
    rayTracer.traceCells( 10, 10, 10+9, 10-9 );
    s << hydroogmap::toText( theMap ) << endl;
}

void testLos()
{
    hydroogmap::OgMap theMap( 20, 20,
                             0, 0, 0,
                             0.1,
                             0 );

    hydroogmap::OgLosTracer losTracer( theMap );

    int x0,y0,x1,y1,xend,yend;
    bool ret;

    x0=3;  y0=3;
    x1=12, y1=y0;

    //
    // Clear trace
    //
    ret = losTracer.isClearCells( x0,y0, x1,y1, xend, yend );
    if ( ret != true )
    {
        cout << "ERROR(testraytrace.cpp): error on clear trace" << endl;
        exit(1);
    }
    if ( xend != x1 || yend != y1 )
    {
        cout << "ERROR(testraytrace.cpp): bad endpoint on clear trace" << endl;
        exit(1);
    }

    //
    // Occupied trace
    //
    theMap.gridCell( 9, y0 ) = hydroogmap::CELL_OCCUPIED;
    ret = losTracer.isClearCells( x0,y0, x1,y1, xend, yend );
    if ( ret != false )
    {
        cout << "ERROR(testraytrace.cpp): error on occupied trace" << endl;
        exit(1);
    }
    if ( xend != 9 || yend != y1 )
    {
        cout << "ERROR(testraytrace.cpp): bad endpoint on occupied trace" << endl;
        exit(1);
    }

    //
    // Trace to edge
    //
    x1 = 25;
    theMap.gridCell( 9, y0 ) = hydroogmap::CELL_EMPTY;
    ret = losTracer.isClearCells( x0,y0, x1,y1, xend, yend );
    if ( ret != false )
    {
        cout << "ERROR(testraytrace.cpp): error on edge trace" << endl;
        exit(1);
    }
    if ( xend != 20 || yend != y1 )
    {
        cout << "ERROR(testraytrace.cpp): bad endpoint on edge trace: " << xend << "," << yend << endl;
        exit(1);
    }
}

int main( int argc, char **argv )
{
    if ( argc != 2 )
    {
        cout << "ERROR(testraytrace.cpp): USAGE: " << argv[0] << " <reference_output_filename> " << endl;
        exit(1);
    }

    // Output everything to a big fat string
    std::stringstream ss;
    testBasic(ss);
    testBorders(ss);
    testLos();

    // Print out the test output
    cout << ss.str();

    // Then check that it matches the reference
    if ( !hydroutil::outputMatchesReferenceFile( ss.str(),
                                                 argv[1] ) )
    {
        cout << "ERROR(testraytrace.cpp): Output differs from reference output." << endl;
        cout << "ERROR(testraytrace.cpp): Test FAILED" << endl;
        exit(1);        
    }
    return 0;
}

