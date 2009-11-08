#include <iostream>
#include <hydrotime/time.h>
#include <assert.h>

using namespace std;
using namespace hydrotime;

int main()
{
    //
    // Test comparisons
    //
    Time tSmall( 0, 100 );
    Time tMedium( 0, 150 );
    Time tBig( 0, 200 );
    Time tBigger( 1, 50 );

    assert( tSmall < tMedium );
    assert( tMedium < tBig );
    assert( tBig < tBigger );

    assert( tSmall <= tMedium );
    assert( tMedium <= tBig );
    assert( tBig <= tBigger );

    assert( tBigger > tBig );
    assert( tBig > tMedium );
    assert( tMedium > tSmall );

    assert( tBigger >= tBig );
    assert( tBig >= tMedium );
    assert( tMedium >= tSmall );

    assert( tSmall == tSmall );
    assert( tSmall >= tSmall );
    assert( tSmall <= tSmall );

    //
    // Test printing
    //
    cout<<"TRACE(hydrotimetest.cpp): tSmall:  " << tSmall << endl;
    cout<<"TRACE(hydrotimetest.cpp): tMedium: " << tMedium << endl;
    cout<<"TRACE(hydrotimetest.cpp): tBig:    " << tBig << endl;
    cout<<"TRACE(hydrotimetest.cpp): tBigger: " << tBigger << endl;

    Time tOneSec( 1, 0 );
    cout<<"TRACE(hydrotimetest.cpp): tOneSec: " << tOneSec << endl;

    //
    // Test additions
    //
    Time t( 0, 100 );
    for ( int i=0; i < 20; i++ )
    {
        const Time tOrig = t;
        const Time dt( 0, 600000 );
        const double dtAsDouble = 0.6;
        const double dtAsNegDouble = -0.6;

        t = tOrig+dt;
        Time tWithDouble = tOrig+dtAsDouble;
        Time tWithNegDouble = tOrig-dtAsNegDouble;

        const double tolerance = 2e-6;
        assert( near( t, tWithDouble, tolerance ) );
        assert( near( t, tWithNegDouble, tolerance ) );
    }

    cout<<"TRACE(timetest.cpp): test PASSED" << endl;
}
