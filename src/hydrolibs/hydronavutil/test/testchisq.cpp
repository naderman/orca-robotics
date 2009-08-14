#include <iostream>
#include <hydronavutil/chisquare.h>
#include <cmath>
#include <cstdlib>

using namespace std;

int main()
{
    double critchi = hydronavutil::critchi( 0.05, 2 );
    double p = hydronavutil::pochisq( critchi, 2 );

    if ( fabs(p-0.05) > 1e-5 )
    {
        cout<<"TRACE(testchisq.cpp): test FAILED:" << endl;
        cout<<"TRACE(testchisq.cpp): p: " << p << endl;
        exit(1);
    }

    cout << "hydronavutil::critchi( 0.05, 1 ): " <<  hydronavutil::critchi( 0.05, 1 ) << endl;
    cout << "hydronavutil::critchi( 0.01, 1 ): " <<  hydronavutil::critchi( 0.01, 1 ) << endl;
    cout << "hydronavutil::critchi( 0.001, 1 ): " <<  hydronavutil::critchi( 0.001, 1 ) << endl;

    cout << "hydronavutil::critchi( 0.05, 2 ): " <<  hydronavutil::critchi( 0.05, 2 ) << endl;
    cout << "hydronavutil::critchi( 0.05, 3 ): " <<  hydronavutil::critchi( 0.05, 3 ) << endl;
    cout << "hydronavutil::critchi( 0.05, 4 ): " <<  hydronavutil::critchi( 0.05, 4 ) << endl;

    cout<<"TRACE(testchisq.cpp): test PASSED" << endl;

    return 0;
}
