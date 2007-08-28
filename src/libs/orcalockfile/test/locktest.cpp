#include <iostream>
#include <orcalockfile/lockfile.h>

using namespace std;
using namespace orcalockfile;

int main( int argc, char **argv )
{
    const char *dev = "/dev/ttyS0";

    LockFile *lock;

    try {
        lock = new LockFile( dev );
    }
    catch ( const std::exception &e )
    {
        cout<<"TRACE(locktest.cpp): Failed to lock: " << e.what() << endl;
        exit(1);
    }

    try {
        LockFile secondLock( dev );
        
        cerr << "Error: double-lock succeeded.";
        exit(1);
    }
    catch ( const std::exception &e )
    {
        cout<<"TRACE(locktest.cpp): Correctly caught exception: " << e.what() << endl;
    }

    delete lock;

    cout<<"TRACE(locktest.cpp): test PASSED!" << endl;
    return 0;
}
