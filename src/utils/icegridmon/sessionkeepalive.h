/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2006-2007 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ICEGRIDMON_SESSIONI_KEEP_ALIVE_H
#define ORCA2_ICEGRIDMON_SESSIONI_KEEP_ALIVE_H

#include <Ice/Ice.h>
#include <IceGrid/Registry.h>

namespace icegridmon
{


class SessionKeepAliveThread : public IceUtil::Thread, public IceUtil::Monitor<IceUtil::Mutex>
{
public:

    SessionKeepAliveThread(const IceGrid::AdminSessionPrx& session, long timeoutSec ) :
	session_(session),
        timeout_(IceUtil::Time::seconds(timeoutSec)),
        destroy_(false)
    {
    }

    virtual void
    run()
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
            }
            catch(const Ice::Exception&)
            {
		break;
            }
        }
    }

    void
    destroy()
    {
        Lock sync(*this);
        destroy_ = true;
        notify();
    }

private:

    IceGrid::AdminSessionPrx session_;
    const IceUtil::Time timeout_;
    bool destroy_;
};

typedef IceUtil::Handle<SessionKeepAliveThread> SessionKeepAliveThreadPtr;

} // namespace

#endif
