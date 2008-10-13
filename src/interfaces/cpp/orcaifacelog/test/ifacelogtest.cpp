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

#include <orcaifacelog/ocm.h>
#include <orcaifacelog/datetime.h>
#include <orcaifacelog/common.h>
#include <orcaifacelog/bros1.h>
#include <orcaifacelog/image.h>
#include <orcaifacelog/rangescanner2d.h>
#include <orcaifacelog/vehicledescription.h>

using namespace std;
// using namespace orcaifacelog;
using namespace ifacelog;

int 
main( int argc, char **argv )
{
    {
        orca::FQInterfaceName in, out;
        in.platform = "alpha";
        in.component = "laser2d";
        in.iface = "laserscanner2d";
        stringstream ss; toLogStream( in, ss );
        cout << "FQInterfaceName\n" << ss.str() << endl << endl;
        fromLogStream( out, ss );
        if ( in != out ) {
            stringstream ss; toLogStream( out, ss );
            cout<<"failed! out: '"<<ss.str()<<"'"<<endl;
            return EXIT_FAILURE;
        }
    }
    {
        orca::Time in, out;
        in.seconds = 233232;
        in.useconds = 35343;
        stringstream ss; toLogStream( in, ss );
        cout << "Time\n" << ss.str() << endl << endl;
        fromLogStream( out, ss );
        if ( in != out ) {
            stringstream ss; toLogStream( out, ss );
            cout<<"failed! out: '"<<ss.str()<<"'"<<endl;
            return EXIT_FAILURE;
        }
    }
    {
        orca::StringSeq in, out;
        in.push_back( "one" );
        in.push_back( "two" );
        in.push_back( "three" );
        stringstream ss; toLogStream( in, ss );
        cout << "StringSeq\n" << ss.str() << endl << endl;
        fromLogStream( out, ss );
        if ( in != out ) {
            stringstream ss; toLogStream( out, ss );
            cout<<"failed! out: '"<<ss.str()<<"'"<<endl;
            return EXIT_FAILURE;
        }
    }
    {
        orca::CartesianPoint2d in, out;
        in.x = 2.43;
        in.y = 14.546;
        stringstream ss; toLogStream( in, ss );
        cout << "CartesianPoint2d\n" << ss.str() << endl << endl;
        fromLogStream( out, ss );
        if ( in != out ) {
            stringstream ss; toLogStream( out, ss );
            cout<<"failed! out: '"<<ss.str()<<"'"<<endl;
            return EXIT_FAILURE;
        }
    }
    {
        orca::Frame2d in, out;
        in.p.x = 2.43;
        in.p.y = 14.546;
        in.o = 0.232;
        stringstream ss; toLogStream( in, ss );
        cout << "Frame2d\n" << ss.str() << endl << endl;
        fromLogStream( out, ss );
        if ( in != out ) {
            stringstream ss; toLogStream( out, ss );
            cout<<"failed! out: '"<<ss.str()<<"'"<<endl;
            return EXIT_FAILURE;
        }
    }
    {
        orca::RangeScanner2dDataPtr in = new orca::RangeScanner2dData;
        orca::RangeScanner2dDataPtr out = new orca::RangeScanner2dData;
        in->minRange = 1.0;
        in->maxRange = 2.0;
        in->fieldOfView = 3.0;
        in->startAngle = 4.0;
        in->ranges.push_back( 1.0 );
        in->ranges.push_back( 2.0 );
        stringstream ss; toLogStream( in, ss );
        cout << "RangeScanner2dDataPtr\n" << ss.str() << endl << endl;
        fromLogStream( out, ss );
        // I think we just need to do an approx. comparison
//         if ( !(*in == *out) ) {
//             stringstream ss; toLogStream( out, ss );
//             cout<<"failed! out: '"<<ss.str()<<"'"<<endl;
//             return EXIT_FAILURE;
//         }
    }
    // this is broken, because of the different derived types
//     {
//         orca::VehicleDescription in;
//         orca::VehicleControlVelocityBicycleDescriptionPtr control = new orca::VehicleControlVelocityBicycleDescription();
//         control->type = orca::VehicleControlVelocityBicycle;
//         control->maxForwardSpeed = 3.5;
//         in.control = control;
//         orca::VehicleGeometryCylindricalDescriptionPtr geometry = new orca::VehicleGeometryCylindricalDescription();
//         geometry->type = orca::VehicleGeometryCylindrical;
//         geometry->radius = .8;
//         in.geometry = geometry;
//         stringstream ss; toLogStream( in, ss );
//         cout << "VehicleDescription\n" << ss.str() << endl << endl;
// 
//         orca::VehicleDescription out;
//         out.control = new orca::VehicleControlVelocityBicycleDescription();
//         out.geometry = new orca::VehicleGeometryCylindricalDescription();
//         fromLogStream( out, ss );
//         if ( in != out ) {
//             stringstream ss; toLogStream( out, ss );
//             cout<<"failed! out: '"<<ss.str()<<"'"<<endl;
//             return EXIT_FAILURE;
//         }
//     }
    return EXIT_SUCCESS;
}
