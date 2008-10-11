/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAICE_CATCHUTILS_H
#define ORCAICE_CATCHUTILS_H

#include <Ice/Ice.h>
#include <gbxutilacfr/substatus.h>

namespace orcaice
{

/*!
This function is normally called from inside the catch block of the component's main loop, i.e. inside
SubsystemThread::run() function. 

Catching the following types results in status fault: {std::exception, std::string, char*, "..."}.

Catching the following types is ignored: {Ice::CommunicatorDestroyedException, orcaice::ComponentDeactivatingException}.

Before returning, this function sleeps for 1 second in order to slow things down in case of a persistent error.

Do not call this function outside of a catch block! In this case, there will be no exception to re-throw
and terminate() will be called, resulting in abort().

Example:
@verbatim
while ( !isStopping() )
{
    try 
    {
        // do your stuff
    }
    catch ( ... ) 
    {
        orcaice::catchMainLoopExceptions( subStatus() );
        // possibly do something else, e.g. re-initialize your hardware driver
    }
}
@endverbatim
*/
void catchMainLoopExceptions( gbxutilacfr::SubStatus& subStatus );

/*!
Catching the following types results in status fault: {std::exception, std::string, char*, "..."}.

Do not call this function outside of a catch block! In this case, there will be no exception to re-throw
and terminate() will be called, resulting in abort().

Returns problem description.

Example:
@verbatim
void MainThread::run()
{
    try
    {
        // do something
    }
    catch ( ... ) {
        orcaice::catchAllExceptions( subStatus(), "initializing hardware driver", sleepIntervalMSec );
    } 
} 
@endverbatim
*/
std::string catchAllExceptions( gbxutilacfr::SubStatus& subStatus, const std::string& activity="" );

/*! 
Same as catchAllExceptions() but also sleeps for specified number of milliseconds before returning.
Use this function to slow execution down in case of a persistent error.

Example:
@verbatim
while ( !isStopping() )
{
    try
    {
        // do something
    }
    catch ( ... ) {
        int sleepIntervalMSec = 1000;
        orcaice::catchAllExceptionsWithSleep( subStatus(), "initializing hardware driver", sleepIntervalMSec );
    } 
} 
@endverbatim
*/
std::string catchAllExceptionsWithSleep( gbxutilacfr::SubStatus& subStatus, const std::string& activity="", int sleepIntervalMSec=1000 );

} // namespace


#endif
