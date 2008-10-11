/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <Ice/Ice.h>
#include "catchutils.h"
#include "exceptions.h"

#include <iostream>
#include <sstream>
using namespace std;

namespace orcaice
{

std::string catchExceptions( gbxutilacfr::Tracer& tracer, const std::string& activity )
{
    stringstream exceptionSS;
    if ( !activity.empty() )
        exceptionSS << "(while " + activity + ") ";

    try {
        // re-throw 
        throw;
    }
    catch ( Ice::CommunicatorDestroyedException & ) {
        // This is OK: it means that the communicator shut down (eg via Ctrl-C)
        // somewhere in mainLoop. Eventually, component will tell us to stop.
        return string();
    }
    catch ( const orcaice::ComponentDeactivatingException & e ) {
        // this is OK.
        return string();
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

    // empty string indicates that the problem can be ignored status-wise.
    if ( !exceptionSS.str().empty() )
        tracer.warning( exceptionSS.str() );

    return exceptionSS.str();
}

std::string catchExceptionsWithSleep( gbxutilacfr::Tracer& tracer, const std::string& activity, 
            int sleepIntervalMSec )
{    
    string problem = catchExceptions( tracer, activity );

    if ( sleepIntervalMSec>0 ) {
        stringstream ss; ss << "Will try again after " << sleepIntervalMSec << " ms";
        tracer.info( ss.str() );
        IceUtil::ThreadControl::sleep(IceUtil::Time::milliSeconds(sleepIntervalMSec));
    }

    return problem;
}

std::string catchExceptionsWithStatus( const std::string& activity, 
            gbxutilacfr::SubStatus& subStatus, gbxutilacfr::SubsystemStatusType newStatus )
{
    stringstream exceptionSS;
    if ( !activity.empty() )
        exceptionSS << "(while " + activity + ") ";

    try {
        // re-throw 
        throw;
    }
    catch ( Ice::CommunicatorDestroyedException & ) {
        // This is OK: it means that the communicator shut down (eg via Ctrl-C)
        // somewhere in mainLoop. Eventually, component will tell us to stop.
        return string();
    }
    catch ( const orcaice::ComponentDeactivatingException & e ) {
        // this is OK.
        return string();
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

    // empty string indicates that the problem can be ignored status-wise.
    if ( !exceptionSS.str().empty() )
        return exceptionSS.str();

    switch ( newStatus )
    {
    case gbxutilacfr::SubsystemStatusFault :
    // We treat Stalled status same as Fault.
    // The right thing would be to through an exception but we are here to catch exceptions
    // not to throw them.
    case gbxutilacfr::SubsystemStatusStalled :
        subStatus.fault( exceptionSS.str() );
        break;
    case gbxutilacfr::SubsystemStatusWarning :
        subStatus.warning( exceptionSS.str() );
        break;
    case gbxutilacfr::SubsystemStatusInitialising :
        subStatus.initialising( exceptionSS.str() );
        break;
    case gbxutilacfr::SubsystemStatusOk :
        subStatus.ok( exceptionSS.str() );
    }

    return exceptionSS.str();
}

std::string catchExceptionsWithStatusAndSleep( const std::string& activity, 
            gbxutilacfr::SubStatus& subStatus, gbxutilacfr::SubsystemStatusType newStatus, 
            int sleepIntervalMSec )
{    
    string problem = catchExceptionsWithStatus( activity, subStatus, newStatus );

    // Slow things down in case of persistent error
    if ( sleepIntervalMSec>0 ) {
        // should we get the tracer in here?
//         stringstream ss; ss << "Will try again after " << sleepIntervalMSec << " ms";
//         tracer.info( ss.str() );
        IceUtil::ThreadControl::sleep(IceUtil::Time::milliSeconds(sleepIntervalMSec));
    }

    return problem;
}

void catchMainLoopExceptions( gbxutilacfr::SubStatus& subStatus )
{
    catchExceptionsWithStatusAndSleep( string("running in main loop"), subStatus, gbxutilacfr::SubsystemStatusFault );
}

} // namespace
