#include <iostream>
#include <orcaqgui3d/osgutil.h>
#include <hydroutil/rand.h>
#include <gbxutilacfr/mathdefs.h>
#include <stdlib.h>

using namespace std;
using namespace orcaqgui3d;

bool near( const Vector3 &v1, const Vector3 &v2, double eps=1e-5 )
{
    return ( NEAR( v1.x(), v2.x(), eps ) &&
             NEAR( v1.y(), v2.y(), eps ) &&
             NEAR( v1.z(), v2.z(), eps ) );
}

bool near( const CoordinateFrame &c1, const CoordinateFrame &c2, double eps=1e-5 )
{
    return ( near( c1.fwd(),  c2.fwd(),  eps ) &&
             near( c1.left(), c2.left(), eps ) &&
             near( c1.up(),   c2.up(),   eps ) &&
             near( c1.pos(),  c2.pos(),  eps ) );
}

void testCoordinateFrame()
{
    cout << "============================================================" << endl;
    cout <<"TRACE(eulerosg.cpp): " << __func__ << endl;
    cout << "============================================================" << endl;

    const int NUM_ITERATIONS = 1000;
    for ( int i=0; i < NUM_ITERATIONS; i++ )
    {
        cout << "===================="<<i<<"=======================" << endl;

        double x     = hydroutil::randNum( -9, 9 );
        double y     = hydroutil::randNum( -9, 9 );
        double z     = hydroutil::randNum( -9, 9 );
        double roll  = hydroutil::randNum( -M_PI/2.0, M_PI/2.0 );
        double pitch = hydroutil::randNum( -M_PI/2.0, M_PI/2.0 );
        double yaw   = hydroutil::randNum( -M_PI/2.0, M_PI/2.0 );
        
        int special = 0;
        if ( i == special++ )
        {
            roll  = 0;
            pitch = 0;
            yaw   = 0;
        }
        if ( i == special++ )
        {
            roll  = 0;
            pitch = 0;
        }
        else if ( i == special++ )
        {
            pitch = 0;
            yaw = 0;
        }
        else if ( i == special++ )
        {
            roll = 0;
            yaw = 0;
        }
        else if ( i == special++ )
        {
            roll = 0;
        }
        else if ( i == special++ )
        {
            pitch = 0;
        }
        else if ( i == special++ )
        {
            yaw = 0;
        }
        cout<<"TRACE(eulerosg.cpp): original euler def'n: " 
            << "x=" << x << ", " 
            << "y=" << y << ", " 
            << "z=" << z << ", " 
            << "r=" << roll*180.0/M_PI << "deg, " 
            << "p=" << pitch*180.0/M_PI << "deg, " 
            << "y=" << yaw*180.0/M_PI << "deg" << endl;

        CoordinateFrame cfOrig( Vector3(x, y, z), roll, pitch, yaw );
        CoordinateFrame cfCopy( cfOrig.pos(),
                                cfOrig.roll(),
                                cfOrig.pitch(),
                                cfOrig.yaw() );

        cout<<"TRACE(eulerosg.cpp): cfOrig: " << toEulerString(cfOrig) << endl;

        if ( !near(cfOrig, cfCopy) )
        {
            cout<<"TRACE(eulerosg.cpp): "<<__func__<<": Matrices don't match:" << endl;
            cout<<"TRACE(eulerosg.cpp): Matrix form:" << endl;
            cout<<"TRACE(eulerosg.cpp): cfOrig:\n" << cfOrig << endl;
            cout<<"TRACE(eulerosg.cpp): cfCopy:\n" << cfCopy << endl;
            cout<<"TRACE(eulerosg.cpp): Euler form:" << endl;
            cout<<"TRACE(eulerosg.cpp): cfOrig:\n" << toEulerString(cfOrig) << endl;
            cout<<"TRACE(eulerosg.cpp): cfCopy:\n" << toEulerString(cfCopy) << endl;
            exit(1);
        }
//         testCoordinateFrameMatrixd( cfOrig );
//         testCoordinateFrameMatrixdQuat( cfOrig );
    }
}

int main()
{
    testCoordinateFrame();

    cout<<"TRACE(eulerosg.cpp): test PASSED" << endl;
    return 0;
}
