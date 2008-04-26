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
#include <orcaifacestring/bros1.h>
#include <orcaifacestring/image.h>
#include <orcaifacestring/rangescanner2d.h>
#include <orcaifacestring/vehicledescription.h>

using namespace std;
using namespace orcaifacestring;

int 
main( int argc, char **argv )
{
    {
        orca::FQInterfaceName obj;
        obj.platform = "alpha";
        obj.component = "laser2d";
        obj.iface = "laserscanner2d";
        cout << "FQInterfaceName\n" << toString( obj ) << endl << endl;
    }
    {
        orca::Time obj;
        obj.seconds = 233232;
        obj.useconds = 35343;
        cout << "Time\n" << toString( obj ) << endl << endl;
    }
    {
        orca::StringSeq obj;
        obj.push_back( "one" );
        obj.push_back( "two" );
        obj.push_back( "three" );
        cout << "StringSeq\n" << toString( obj ) << endl << endl;
    }
    {
        orca::CartesianPoint2d cp;
        cp.x = 2.43;
        cp.y = 14.546;
        cout << "CartesianPoint2d\n" << toString( cp ) << endl << endl;

        orca::Frame2d obj;
        obj.p = cp;
        obj.o = 0.232;
        cout << "Frame2d\n" << toString( obj ) << endl << endl;
    }
    {
        orca::RangeScanner2dDataPtr obj = new orca::RangeScanner2dData;
        obj->ranges.push_back( 1.0 );
        obj->ranges.push_back( 2.0 );
        cout << "RangeScanner2dDataPtr\n" << toString( obj ) << endl << endl;
    }
    {
        orca::VehicleDescription obj;
        orca::VehicleControlVelocityBicycleDescriptionPtr control = new orca::VehicleControlVelocityBicycleDescription();
        control->type = orca::VehicleControlVelocityBicycle;
        control->maxForwardSpeed = 3.5;
        obj.control = control;
        orca::VehicleGeometryCylindricalDescriptionPtr geometry = new orca::VehicleGeometryCylindricalDescription();
        geometry->type = orca::VehicleGeometryCylindrical;
        geometry->radius = .8;
        obj.geometry = geometry;
        cout << "VehicleDescription\n" << toString( obj ) << endl << endl;
    }
    return EXIT_SUCCESS;
}
