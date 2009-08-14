#include <iostream>
#include <hydronavutil/hydronavutil.h>
#include <hydroutil/rand.h>

using namespace std;

//
// Tests that adding and subtracting pose offsets are inverses.
//

bool isNear( const hydronavutil::Pose &p,
             const hydronavutil::Pose &q )
{
    const double EPS = 1e-5;
    return ( fabs( p.x()-q.x() ) < EPS &&
             fabs( p.y()-q.y() ) < EPS &&
             fabs( p.theta()-q.theta() ) < EPS );             
}

hydronavutil::Pose randomPose()
{
    return hydronavutil::Pose( hydroutil::randNum( -100, 100 ),
                               hydroutil::randNum( -100, 100 ),
                               hydroutil::randNum( -M_PI, M_PI ) );
}

int main()
{
    int NUM_TIMES = 10000;
    for ( int i=0; i < NUM_TIMES; i++ )
    {
        //
        // Test add/subtract pose offset
        //
        {
            hydronavutil::Pose p, q;
            if ( i == 0 )
            {
                p = hydronavutil::Pose( 1, 1, 30*M_PI/180.0 );
                q = hydronavutil::Pose( 2, 1, 30*M_PI/180.0 );
            }
            else
            {
                p = randomPose();
                q = randomPose();
            }

            hydronavutil::Pose added, subtracted;

            const bool normaliseHeading = true;
            hydronavutil::addPoseOffset( p, q, added, normaliseHeading );
            hydronavutil::subtractFinalPoseOffset( added, q, subtracted, normaliseHeading );

            if ( !isNear( p, subtracted ) )
            {
                cout<<"TRACE(testoffset.cpp): i: " << i << endl;
                cout << "ERROR(testoffset.cpp): p:          " << p << endl;
                cout << "ERROR(testoffset.cpp): q:          " << q << endl;
                cout << "ERROR(testoffset.cpp): added:      " << added << endl;
                cout << "ERROR(testoffset.cpp): subtracted: " << subtracted << endl;
                cout<<"TRACE(testoffset.cpp): test FAILED" << endl;
                exit(1);
            }
        }

        //
        // test inverse transform
        //
        {
            hydronavutil::Pose p, tx;
            if ( i == 0 )
            {
                p  = hydronavutil::Pose( 1, 1, 30*M_PI/180.0 );
                tx = hydronavutil::Pose( 2, 1, 30*M_PI/180.0 );
            }
            else
            {
                p  = randomPose();
                tx = randomPose();
            }            


            hydronavutil::Pose invTx;
            hydronavutil::getInverseTransform( tx.x(), tx.y(), tx.theta(),
                                               invTx.x(), invTx.y(), invTx.theta() );

            hydronavutil::Pose added, subtracted;
            const bool normaliseHeading = true;
            hydronavutil::addPoseOffset( p, tx, added, normaliseHeading );
            hydronavutil::addPoseOffset( added, invTx, subtracted, normaliseHeading );

            if ( !isNear( p, subtracted ) )
            {
                cout<<"TRACE(testoffset.cpp): i: " << i << endl;
                cout << "ERROR(testoffset.cpp): p:          " << p << endl;
                cout << "ERROR(testoffset.cpp): tx:         " << tx << endl;
                cout << "ERROR(testoffset.cpp): invTx:      " << invTx << endl;
                cout << "ERROR(testoffset.cpp): added:      " << added << endl;
                cout << "ERROR(testoffset.cpp): subtracted: " << subtracted << endl;
                cout << "ERROR(testoffset.cpp): testFAILED" << endl;
                exit(1);
            }
        }
    }

    cout<<"TRACE(testoffset.cpp): test PASSED" << endl;
    return 0;
}
