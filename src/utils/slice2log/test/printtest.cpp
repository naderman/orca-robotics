/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#include <orcaifacestring/datetime.h>
#include <orcaifacestring/bros1.h>
#include <orcaifacestring/rangescanner2d.h>

#include <iostream>

using namespace std;

int 
main( int argc, char **argv )
{
    orca::Time t;
    t.seconds = 123;
    t.useconds = 45;
    cout<<ifacestring::toString( t )<<endl<<endl;

    orca::Frame2d f;
    f.p.x = 1.2;
    f.p.y = 3.4;
    f.o = 5.6;
    cout<<ifacestring::toString( f )<<endl<<endl;

    orca::RangeScanner2dDataPtr scan = new orca::RangeScanner2dData();
    scan->timeStamp = t;
    scan->minRange = 0;
    scan->maxRange = 33;
    scan->fieldOfView = 77;
    scan->startAngle = 13;
    scan->ranges.push_back( 3. );
    scan->ranges.push_back( 13. );
    cout<<ifacestring::toString( scan )<<endl<<endl;

    cout<<ifacestring::toString( scan, 0 )<<endl<<endl;
    cout<<ifacestring::toString( scan, 1 )<<endl<<endl;

    cout<<ifacestring::toString( scan, 1000, 0 )<<endl<<endl;
    return EXIT_SUCCESS;
}
