#include <iostream>
#include <assert.h>
#include <orcageom2d/geom2d.h>
#include <hydroutil/mathdefs.h>
#include <hydroutil/rand.h>

using namespace std;
using namespace geom2d;

const double EPS=1e-5;

int main()
{
    Point  c1,c2;
    double r1,r2;
    Point  p,q;
    bool intersect;

    cout << endl << "Circles on the x-axis" << endl << endl;
    c1.x() = -3;
    c2.x() = 3;
    c1.y() = 0;
    c2.y() = 0;
    r1 = 6;
    r2 = 6;

    cout<<"TRACE(circle.cpp): circle 1: centre "<<c1<<", radius "<<r1<<endl;
    cout<<"TRACE(circle.cpp): circle 2: centre "<<c2<<", radius "<<r2<<endl;
    intersect = circleCircleIntersection( c1,r1, c2,r2, p,q );
    assert( intersect );
    cout<<"TRACE(circle.cpp): p: " << p << endl;
    cout<<"TRACE(circle.cpp): q: " << q << endl;
    assert( NEAR(p.x(),0,EPS) );
    assert( NEAR(q.x(),0,EPS) );
    assert( NEAR(q.y(),-p.y(),EPS) );

    cout << endl << "Circles on the y-axis" << endl << endl;
    c1.x()=0;
    c2.x()=0;
    c1.y()=-1;
    c2.y()=1;

    cout<<"TRACE(circle.cpp): circle 1: centre "<<c1<<", radius "<<r1<<endl;
    cout<<"TRACE(circle.cpp): circle 2: centre "<<c2<<", radius "<<r2<<endl;
    intersect = circleCircleIntersection( c1,r1, c2,r2, p,q );
    assert( intersect );
    cout<<"TRACE(circle.cpp): p: " << p << endl;
    cout<<"TRACE(circle.cpp): q: " << q << endl;
    assert( NEAR(p.y(),0,EPS) );
    assert( NEAR(q.y(),0,EPS) );
    assert( NEAR(q.x(),-p.x(),EPS) );

    cout << endl << "Non-intersecting Circles" << endl << endl;
    c1.y()=100;
    c2.y()=-100;
    cout<<"TRACE(circle.cpp): circle 1: centre "<<c1<<", radius "<<r1<<endl;
    cout<<"TRACE(circle.cpp): circle 2: centre "<<c2<<", radius "<<r2<<endl;
    intersect = circleCircleIntersection( c1,r1, c2,r2, p,q );
    assert( !intersect );

    cout << endl << "Co-Incident Circles" << endl << endl;
    c1.y()=100;
    c2.y()=100;
    cout<<"TRACE(circle.cpp): circle 1: centre "<<c1<<", radius "<<r1<<endl;
    cout<<"TRACE(circle.cpp): circle 2: centre "<<c2<<", radius "<<r2<<endl;
    try {
        intersect = circleCircleIntersection( c1,r1, c2,r2, p,q );
        assert( false && "should throw" );
    } catch ( std::string & ) {}


    cout << endl << "Random Circles" << endl << endl;
    const int NUM_TIMES = 10000;
    for ( int i=0; i < NUM_TIMES; i++ )
    {
        // cout << endl;

        c1.x() = hydroutil::randNum( -10, 10 );
        c1.y() = hydroutil::randNum( -10, 10 );
        c2.x() = hydroutil::randNum( -10, 10 );
        c2.y() = hydroutil::randNum( -10, 10 );
        r1 = hydroutil::randNum( 0.1, 5 );
        r2 = hydroutil::randNum( 0.1, 5 );

//         cout<<"TRACE(circle.cpp): circle 1: centre "<<c1<<", radius "<<r1<<endl;
//         cout<<"TRACE(circle.cpp): circle 2: centre "<<c2<<", radius "<<r2<<endl;
        intersect = circleCircleIntersection( c1,r1, c2,r2, p,q );
        if ( intersect )
        {

//             cout<<"TRACE(circle.cpp): dist(p,c1)  ::  dist(q,c1): " << dist(p,c1) << "  ::  "<<dist(q,c1) << endl;
//             cout<<"TRACE(circle.cpp): dist(p,c2)  ::  dist(q,c2): " << dist(p,c2) << "  ::  "<<dist(q,c2) << endl;

//             cout<<"TRACE(circle.cpp): dist(p,c1): " << dist(p,c1) << " [r1="<<r1<<"]" << endl;
//             cout<<"TRACE(circle.cpp): dist(p,c2): " << dist(p,c2) << " [r2="<<r2<<"]" << endl;

            // p & q equidistant from endpoints
            assert( NEAR( dist(p,c1), dist(q,c1), 1e-3 ) );
            assert( NEAR( dist(p,c2), dist(q,c2), 1e-3 ) );

            // p & q on circles
            assert( NEAR( dist(p,c1), r1, 1e-3 ) );
            assert( NEAR( dist(p,c2), r2, 1e-3 ) );
        }
    }

    cout << "Test PASSED" << endl;
    return 0;
}
