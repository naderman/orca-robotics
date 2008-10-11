/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#include <orcaifaceinit/datetime.h>
#include <orcaifaceinit/bros1.h>
#include <orcaifaceinit/rangescanner2d.h>

#include <iostream>

using namespace std;

int 
main( int argc, char **argv )
{
    orca::Time t;
    ifaceinit::zeroAndClear( t );  
    cout<<"setting Time ... ";
    if ( t.seconds || t.useconds ) {
        cout<<"failed"<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;

    t.seconds = 123;
    t.useconds = 45;
    ifaceinit::zeroAndClear( t );    
    cout<<"resetting Time ... ";
    if ( t.seconds || t.useconds ) {
        cout<<"failed"<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;

    orca::Frame2d f;
    ifaceinit::zeroAndClear( f );  
    cout<<"setting Frame2d ... ";
    if ( f.p.x || f.p.y || f.o ) {
        cout<<"failed"<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;

    f.p.x = 1.2;
    f.p.y = 3.4;
    f.o = 5.6;
    ifaceinit::zeroAndClear( f );  
    cout<<"resetting Frame2d ... ";
    if ( f.p.x || f.p.y || f.o ) {
        cout<<"failed"<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;

    orca::RangeScanner2dDataPtr scan = new orca::RangeScanner2dData();
    ifaceinit::zeroAndClear( scan );
    cout<<"setting RangeScanner2dData ... ";
    if ( scan->timeStamp.seconds || scan->minRange || scan->ranges.size() ) {
        cout<<"failed"<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;

    scan->timeStamp = t;
    scan->minRange = 0;
    scan->maxRange = 33;
    scan->fieldOfView = 77;
    scan->startAngle = 13;
    scan->ranges.push_back( 3. );
    scan->ranges.push_back( 13. );
    ifaceinit::zeroAndClear( scan );
    cout<<"resetting RangeScanner2dData ... ";
    if ( scan->timeStamp.seconds || scan->minRange || scan->ranges.size() ) {
        cout<<"failed"<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;

    return EXIT_SUCCESS;
}
