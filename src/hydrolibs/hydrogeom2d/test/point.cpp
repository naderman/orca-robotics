#include <cstdlib>
#include <iostream>
#include <hydrogeom2d/geom2d.h>
#include <hydroutil/rand.h>

using namespace std;

bool close( const geom2d::Point &p, const geom2d::Point &q )
{
    return ( fabs(p.x()-q.x()) < 1e-5 && 
             fabs(p.y()-q.y()) < 1e-5 );
}

void
testTransformations()
{
    const int NUM_TESTS = 1000;

    geom2d::Point tp, ttp;

    for ( int i=0; i < NUM_TESTS; i++ )
    {
        geom2d::Point p( hydroutil::randNum( -1000, 1000 ),
                         hydroutil::randNum( -1000, 1000 ) );

        double x = hydroutil::randNum( -1000, 1000 );
        double y = hydroutil::randNum( -1000, 1000 );
        double theta = hydroutil::randNum( -M_PI, M_PI );

//        cout<<"TRACE(point.cpp): p: " << p << endl;

        geom2d::rotate( p, theta, tp );
        geom2d::rotate( tp, -theta, ttp );
        if ( !close( p, ttp ) )
        {
            cout<<"TRACE(point.cpp): rotate test failed." << endl;
            cout<<"TRACE(point.cpp): p:   " << p << endl;
            cout<<"TRACE(point.cpp): tp:  " << tp << endl;
            cout<<"TRACE(point.cpp): ttp: " << ttp << endl;
            exit(1);
        }

        geom2d::addTransform( p, x, y, theta, tp );
        geom2d::subtractTransform( tp, x, y, theta, ttp );
        if ( !close( p, ttp ) )
        {
            cout<<"TRACE(point.cpp): transform test failed." << endl;
            cout<<"TRACE(point.cpp): p:   " << p << endl;
            cout<<"TRACE(point.cpp): tp:  " << tp << endl;
            cout<<"TRACE(point.cpp): ttp: " << ttp << endl;
            exit(1);
        }
    }
}

int main()
{
    testTransformations();

    cout<<"TRACE(line.cpp): test PASSED" << endl;
    return 0;
}
