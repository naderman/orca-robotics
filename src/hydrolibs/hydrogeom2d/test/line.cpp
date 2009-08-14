#include <cstdlib>
#include <iostream>
#include <hydrogeom2d/geom2d.h>

using namespace std;

void
testPointLineDistance()
{
    // Set up a scenario where the distances are obvious
    geom2d::Point p1( 1, 0 );
    geom2d::Point linePoint1( 2, -1 );
    geom2d::Point linePoint2( 2,  1 );
    geom2d::Point p2( 3, 0 );

    const double trueDistance = 1.0;

    // then rotate it
    int NUM_ROTATIONS = 10;
    for ( int i=0; i < NUM_ROTATIONS; i++ )
    {
        double theta = (double)i/(double)NUM_ROTATIONS * 2*M_PI;

        geom2d::Point rp1, rp2, rLinePoint1, rLinePoint2;

        geom2d::rotate( p1, theta, rp1 );
        geom2d::rotate( linePoint1, theta, rLinePoint1 );
        geom2d::rotate( linePoint2, theta, rLinePoint2 );
        geom2d::rotate( p2, theta, rp2 );

        double d1 = geom2d::dist( geom2d::Line( rLinePoint1, rLinePoint2 ), rp1 );
        double d2 = geom2d::dist( geom2d::Line( rLinePoint1, rLinePoint2 ), rp2 );

        const double EPS = 1e-9;
        if ( fabs( trueDistance-d1 ) > EPS ||
             fabs( trueDistance-d2 ) > EPS )
        {
            cout<<"TRACE(line.cpp): rp1: " << rp1 << endl;
            cout<<"TRACE(line.cpp): rp2: " << rp2 << endl;
            cout<<"TRACE(line.cpp): linePoint1: " << linePoint1 << endl;
            cout<<"TRACE(line.cpp): linePoint2: " << linePoint2 << endl;
            cout<<"TRACE(line.cpp): trueDistance: " << trueDistance << endl;
            cout<<"TRACE(line.cpp): d1: " << d1 << endl;
            cout<<"TRACE(line.cpp): d2: " << d2 << endl;
            cout<<"TRACE(line.cpp): " << endl;
            cout<<"TRACE(line.cpp): test FAILED" << endl;
            exit(1);
        }
    }
}

int main()
{
    testPointLineDistance();

    cout<<"TRACE(line.cpp): test PASSED" << endl;
    return 0;
}
