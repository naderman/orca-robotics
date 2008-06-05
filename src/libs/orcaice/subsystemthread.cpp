/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <sstream>

#include "subsystemthread.h"
#include "exceptions.h"
#include <Ice/Ice.h>

using namespace orcaice;
using namespace std;


SubsystemThread::SubsystemThread( gbxutilacfr::Tracer& tracer, gbxutilacfr::Status& status, const std::string& subsysName ) :
    tracer_(tracer),
    subStatus_( status, subsysName )
{
}

void 
SubsystemThread::run()
{
    stringstream ss;
    try
    {
        walk();
    }
    catch ( const Ice::CommunicatorDestroyedException & )
    {
        // This is OK: it means that the communicator shut down (eg via Ctrl-C)
    }
    catch ( const orcaice::ComponentDeactivatingException& )
    {
        // This is OK: it means the component was told to shutdown
    }
    catch ( const IceUtil::Exception &e )
    {
        ss << "SubsystemThread::run() "<<subsysName()<<": Caught unexpected exception: " << e;
    }
    catch ( const std::exception &e )
    {
        ss << "SubsystemThread::run() "<<subsysName()<<": Caught unexpected exception: " << e.what();
    }
    catch ( const std::string &e )
    {
        ss << "SubsystemThread::run() "<<subsysName()<<": Caught unexpected string: " << e;
    }
    catch ( const char *e )
    {
        ss << "SubsystemThread::run() "<<subsysName()<<": Caught unexpected char *: " << e;
    }
    catch ( ... )
    {
        ss << "SubsystemThread::run() "<<subsysName()<<": Caught unexpected unknown exception.";
    }

    // report status fault if there was an exception and we are not stopping
    if ( !ss.str().empty() ) {
        if ( !isStopping() ) {
            tracer_.error( ss.str() );
            subStatus().fault( ss.str() );
        }
        else {
            tracer_.warning( subsysName()+": Caught exception when stopping: "+ss.str() );
        }
    }
    else {
        tracer_.info( subsysName()+": dropping out from run() " );
    }

    // wait for somebody to realize that we are quitting and tell us to stop.
    waitForStop();
}
