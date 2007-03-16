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

#include <IceGrid/Registry.h>
#include <orcaice/context.h>
#include <IceUtil/Thread.h>

namespace icegridmon
{


class SessionKeepAliveThread : public IceUtil::Thread, public IceUtil::Monitor<IceUtil::Mutex>
{
public:

    SessionKeepAliveThread( const IceGrid::AdminSessionPrx &session, 
                            long timeoutSec, 
                            const orcaice::Context& context );

    virtual void run();

    void destroy();

private:

    IceGrid::AdminSessionPrx session_;
    const IceUtil::Time timeout_;
    orcaice::Context context_;
    bool destroy_;
};

typedef IceUtil::Handle<SessionKeepAliveThread> SessionKeepAliveThreadPtr;

} // namespace

#endif
