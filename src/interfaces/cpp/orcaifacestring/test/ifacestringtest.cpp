/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#include <iostream>

#include <orcaifacestring/ocm.h>
#include <orcaifacestring/datetime.h>
#include <orcaifacestring/orca.h>
// #include <orcaifacestring/bros1.h>
// #include <orcaifacestring/image.h>
// #include <orcaifacestring/rangescanner2d.h>
// #include <orcaifacestring/vehicledescription.h>

using namespace std;
using namespace orcaifacestring;

int 
main( int argc, char **argv )
{
    orca::FQInterfaceName n;
    n.platform = "alpha";
    n.component = "laser2d";
    n.iface = "laserscanner2d";
    cout << toString( n ) << endl;

    orca::Time t;
    t.seconds = 233232;
    t.useconds = 35343;
    cout << toString( t ) << endl;

    orca::StringSeq ss;
    ss.push_back( "one" );
    ss.push_back( "two" );
    ss.push_back( "three" );
    cout << toString( ss ) << endl;

//     orca::CartesianPoint2d cp;
//     cp.x = 2.43;
//     cp.y = 14.546;
//     cout << toString( cp ) << endl;
// 
//     orca::Frame2d f;
//     f.p = cp;
//     f.o = 0.232;
//     cout << toString( f ) << endl;
// 
//     orca::RangeScanner2dDataPtr r = new orca::RangeScanner2dData;
//     r->ranges.push_back( 1.0 );
//     r->ranges.push_back( 2.0 );
//     cout << toString( r ) << endl;
// 
//     orca::VehicleDescription vd;
//     orca::VehicleControlVelocityBicycleDescriptionPtr control = new orca::VehicleControlVelocityBicycleDescription();
//     control->type = orca::VehicleControlVelocityBicycle;
//     control->maxForwardSpeed = 3.5;
//     vd.control = control;
//     orca::VehicleGeometryCylindricalDescriptionPtr geometry = new orca::VehicleGeometryCylindricalDescription();
//     geometry->type = orca::VehicleGeometryCylindrical;
//     geometry->radius = .8;
//     vd.geometry = geometry;
//     cout << toString( vd ) << endl;

    return EXIT_SUCCESS;
}
