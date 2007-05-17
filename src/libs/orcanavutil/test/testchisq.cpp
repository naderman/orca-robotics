#include <iostream>
#include <orcanavutil/chisquare.h>
#include <cmath>

using namespace std;

int main()
{
    double critchi = orcanavutil::critchi( 0.05, 2 );
    double p = orcanavutil::pochisq( critchi, 2 );

    if ( fabs(p-0.05) > 1e-5 )
    {
        cout<<"TRACE(testchisq.cpp): test FAILED:" << endl;
        cout<<"TRACE(testchisq.cpp): p: " << p << endl;
        exit(1);
    }

    cout<<"TRACE(testchisq.cpp): test PASSED" << endl;

    return 0;
}
