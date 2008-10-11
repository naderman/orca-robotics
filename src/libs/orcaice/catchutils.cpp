/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include "catchutils.h"
#include "exceptions.h"

#include <iostream>
using namespace std;

namespace orcaice
{

void catchMainLoopExceptions( gbxutilacfr::SubStatus& subStatus )
{
    stringstream exceptionSS;
    exceptionSS << "(while running inside the main component loop) ";

    try {
        // re-throw 
        throw;
    }
    catch ( Ice::CommunicatorDestroyedException & ) {
        // This is OK: it means that the communicator shut down (eg via Ctrl-C)
        // somewhere in mainLoop. Eventually, component will tell us to stop.
    }
    catch ( const orcaice::ComponentDeactivatingException & e ) {
        // this is OK.
    }
    catch ( const std::exception &e ) {
        exceptionSS << "caught unexpected exception: " << e.what();
    }
    catch ( const std::string &e ) {
        exceptionSS << "caught unexpected string: " << e;
    }
    catch ( const char *e ) {
        exceptionSS << "caught unexpected char *: " << e;
    }
    catch ( ... ) {
        exceptionSS << "caught unexpected unknown exception.";
    }

    subStatus.fault( exceptionSS.str() ); 

    // Slow things down in case of persistent error
    IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(1));
}


std::string catchAllExceptions( gbxutilacfr::SubStatus& subStatus, const std::string& activity )
{
    stringstream exceptionSS;
    if ( !activity.empty() )
        exceptionSS << "(while " + activity + ") ";

    try {
        // re-throw 
        throw;
    }
    catch ( const std::exception &e ) {
        exceptionSS << "caught unexpected exception: " << e.what();
    }
    catch ( const std::string &e ) {
        exceptionSS << "caught unexpected string: " << e;
    }
    catch ( const char *e ) {
        exceptionSS << "caught unexpected char *: " << e;
    }
    catch ( ... ) {
        exceptionSS << "caught unexpected unknown exception.";
    }

    subStatus.fault( exceptionSS.str() ); 

    return exceptionSS.str();
}

std::string catchAllExceptionsWithSleep( gbxutilacfr::SubStatus& subStatus, const std::string& activity, int sleepIntervalMSec )
{    
    string problem = catchAllExceptions( subStatus, activity );

    // Slow things down in case of persistent error
    if ( sleepIntervalMSec>0 ) {
        // should we get the tracer in here?
//         stringstream ss; ss << "Will try again after " << sleepIntervalMSec << " ms";
//         tracer.info( ss.str() );
        IceUtil::ThreadControl::sleep(IceUtil::Time::milliSeconds(sleepIntervalMSec));
    }

    return problem;
}

} // namespace
