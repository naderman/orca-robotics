/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_TRACERMON_NETWORK_HANDLER_H
#define ORCA2_TRACERMON_NETWORK_HANDLER_H

#include <orcaice/thread.h>
#include <orcaice/context.h>
#include <orcaice/eventqueue.h>
#include "user.h"
#include "network.h"

namespace tracermon
{

class NetworkHandler : public orcaice::Thread, public Network //, public orca::TracerConsumer
{
public:

    NetworkHandler( User* user,
                    const orcaice::Context & context );
    virtual ~NetworkHandler();

    // from thread
    virtual void run();
    
    // from Network
    virtual void setVerbosityLevel( int error, int warn=-1, int info=-1, int debug=-1 );

    // from consumer
//     virtual void setData(const orca::TracerData& data, const Ice::Current&);

private:

    User* user_;
    orcaice::EventQueuePtr events_;

    orcaice::Context context_;

    void setRemoteVerbosity();

};

} // namespace

#endif
