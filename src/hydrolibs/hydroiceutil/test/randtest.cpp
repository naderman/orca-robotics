#include <hydroiceutil/rand.h>
#include <iostream>
using namespace std;

int main()
{
    const int NUM = 10000000;

    const double minVal = 0.0;
    const double maxVal = 1.0;

    for ( int i=0; i < NUM; i++ )
    {
        double r    = hydroutil::randNum( minVal, maxVal );
        double rInc = hydroutil::randNumInclusive( minVal, maxVal );

        if ( !( r >= minVal && r < maxVal ) )
        {
            cout << "ERROR(rand.cpp): randNum failed.  Produced: " << r << endl;
            exit(1);
        }
        if ( !( rInc >= minVal && rInc <= maxVal ) )
        {
            cout << "ERROR(rand.cpp): randNumInclusive failed.  Produced: " << rInc << endl;
            exit(1);
        }
    }

//     // note: if rand() returns RAND_MAX:
//     double r = (RAND_MAX / ( RAND_MAX + 1.0));
//     if ( r == 1.0 )
//     {
//         cout << "Oops: randNum can return the top of the range!" << endl;
//         cout<<"TRACE(rand.cpp): RAND_MAX: " << RAND_MAX << endl;
//         exit(1);
//     }

    cout<<"TRACE(rand.cpp): Test PASSED." << endl;
    return 0;
}
