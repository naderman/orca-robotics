/*
 * Orca-Robotics Project: Components for robotics
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include "catchutils.h"
#include <IceUtil/IceUtil.h>

// #include <iostream>
#include <sstream>
using namespace std;

namespace hydroiceutil
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
            gbxutilacfr::SubHealth& subHealth,
            gbxutilacfr::SubsystemHealth newHealth )
{
    stringstream exceptionSS;

    string fullActivity;
    if ( !activity.empty() )
        fullActivity = "(while " + activity + ") ";

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

    switch ( newHealth )
    {
    case gbxutilacfr::SubsystemFault :
        subHealth.fault( fullActivity + exceptionSS.str() );
        break;
    case gbxutilacfr::SubsystemWarning :
        subHealth.warning( fullActivity + exceptionSS.str() );
        break;
    case gbxutilacfr::SubsystemOk :
        subHealth.ok( fullActivity + exceptionSS.str() );
        break;
    default :
        assert( !"Unknown subsystem health type" );
    }

    return ( fullActivity + exceptionSS.str() );
}

std::string catchExceptionsWithStatusAndSleep( const std::string& activity,
            gbxutilacfr::SubHealth& subHealth,
            gbxutilacfr::SubsystemHealth newHealth,
            int sleepIntervalMSec )
{
    string problem = catchExceptionsWithStatus( activity, subHealth, newHealth );

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
