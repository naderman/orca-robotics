/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <IceUtil/IceUtil.h>

#include "safethread.h"

using namespace orcaice;
using namespace std;

void 
SafeThread::run()
{
    try
    {
        walk();
    }
    catch ( const IceUtil::Exception &e )
    {
        cout<<"SafeThread::run(): Caught unexpected exception: " << e << endl;
    }
    catch ( const std::exception &e )
    {
        cout << "SafeThread::run(): Caught unexpected exception: " << e.what() << endl;
    }
    catch ( const std::string &e )
    {
        cout << "SafeThread::run(): Caught unexpected string: " << e << endl;
    }
    catch ( const char *e )
    {
        cout << "SafeThread::run(): Caught unexpected char *: " << e << endl;
    }
    catch ( ... )
    {
        cout << "SafeThread::run(): Caught unexpected unknown exception.";
    }

    // wait for the component to realize that we are quitting and tell us to stop.
    waitForStop();
}
