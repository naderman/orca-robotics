/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2006-2007 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <sstream>

#include "sessionkeepalive.h"

using namespace std;
using namespace icegridmon;


SessionKeepAliveThread::SessionKeepAliveThread(
                const IceGrid::AdminSessionPrx& session, 
                long timeoutSec, 
                const orcaice::Context& context ) :
    session_(session),
    timeout_(IceUtil::Time::seconds(timeoutSec)),
    context_(context),
    destroy_(false)
{
}

void
SessionKeepAliveThread::run()
{
    Lock sync(*this);
    while(!destroy_)
    {
        timedWait(timeout_);
        if(destroy_)
        {
            break;
        }
        try
        {
            session_->keepAlive();

//             context_.tracer()->debug( "Keeping session alive", 6 );
        }
        catch( const Ice::Exception& e )
        {
            stringstream ss; ss<<"Failed to keep session alive: "<<e;
            context_.tracer()->warning( ss.str() );
        }
    }
}

void
SessionKeepAliveThread::destroy()
{
    Lock sync(*this);
    destroy_ = true;
    notify();
}

