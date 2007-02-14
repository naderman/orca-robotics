/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp, George Mathews
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
// Ensure that assertions are compiled in
#undef NDEBUG

#include <iostream>
#include <sstream>
#include <orcaogmap/orcaogmap.h>
#include <orcamisc/diff.h>

using namespace std;
using namespace orcaogmap;

class MarkCellTracer
{
public:
    MarkCellTracer(orcaogmap::OgMap &theMap)
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

    orcaogmap::OgMap &theMap_;
    int endCellX_, endCellY_;
};

void testBasic( std::ostream &s )
{
    orcaogmap::OgMap theMap;
    theMap.offset().o = 0;
    theMap.reallocate( 20, 20 );
    theMap.fill( 0 );

    MarkCellTracer t(theMap);
    OgRayTracer<MarkCellTracer> rayTracer( t );

    rayTracer.trace( 10, 10, 10+9, 10+5 );
    rayTracer.trace( 10, 10, 10+5, 10+9 );
    rayTracer.trace( 10, 10, 10-5, 10+9 );
    rayTracer.trace( 10, 10, 10-9, 10+5 );
    rayTracer.trace( 10, 10, 10-9, 10-5 );
    rayTracer.trace( 10, 10, 10-5, 10-9 );
    rayTracer.trace( 10, 10, 10+5, 10-9 );
    rayTracer.trace( 10, 10, 10+9, 10-5 );

    rayTracer.trace( 0, 10, 19, 9 );

    s << orcaogmap::toText( theMap ) << endl;
}

void testBorders( std::ostream &s )
{
    orcaogmap::OgMap theMap;
    theMap.offset().o = 0;
    theMap.reallocate( 20, 20 );
    theMap.fill( 0 );

    MarkCellTracer t(theMap);
    OgRayTracer<MarkCellTracer> rayTracer( t );

    rayTracer.trace( 10, 10, 10+9, 10+0 );
    s << orcaogmap::toText( theMap ) << endl;
    rayTracer.trace( 10, 10, 10+9, 10+9 );
    s << orcaogmap::toText( theMap ) << endl;
    rayTracer.trace( 10, 10, 10+0, 10+9 );
    s << orcaogmap::toText( theMap ) << endl;
    rayTracer.trace( 10, 10, 10-9, 10+9 );
    s << orcaogmap::toText( theMap ) << endl;
    rayTracer.trace( 10, 10, 10-9, 10-0 );
    s << orcaogmap::toText( theMap ) << endl;
    rayTracer.trace( 10, 10, 10-9, 10-9 );
    s << orcaogmap::toText( theMap ) << endl;
    rayTracer.trace( 10, 10, 10+0, 10-9 );
    s << orcaogmap::toText( theMap ) << endl;
    rayTracer.trace( 10, 10, 10+9, 10-9 );
    s << orcaogmap::toText( theMap ) << endl;
}

void testLos()
{
    orcaogmap::OgMap theMap;
    theMap.offset().o = 0;
    theMap.reallocate( 20, 20 );
    theMap.fill( 0 );

    OgLosTracer losTracer( theMap );

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
    theMap.gridCell( 9, y0 ) = orcaogmap::CELL_OCCUPIED;
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
    theMap.gridCell( 9, y0 ) = orcaogmap::CELL_EMPTY;
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
    if ( !orcamisc::outputMatchesReferenceFile( ss.str(),
                                                argv[1] ) )
    {
        cout << "ERROR(testraytrace.cpp): Output differs from reference output." << endl;
        cout << "ERROR(testraytrace.cpp): Test FAILED" << endl;
        exit(1);        
    }
    return 0;
}

