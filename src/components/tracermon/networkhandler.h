/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_TRACERMON_NETWORK_HANDLER_H
#define ORCA2_TRACERMON_NETWORK_HANDLER_H

#include <orca/tracer.h>
#include <orcaiceutil/safethread.h>
#include <orcaice/context.h>
#include <orcaiceutil/eventqueue.h>
#include "user.h"
#include "network.h"

namespace tracermon
{

// class NetworkHandler : public Network
class NetworkHandler : public orcaiceutil::SafeThread, public Network //, virtual public orca::TracerConsumer
{
public:

    NetworkHandler( User* user,
                    const orcaice::Context & context );
    virtual ~NetworkHandler();

    // from thread
    virtual void walk();
    
    // from Network
    virtual void setVerbosityLevel( int error, int warn, int info, int debug );

    // from consumer
//     virtual void setData(const orca::TracerData& data, const Ice::Current&);

private:

    User* user_;
    orcaiceutil::EventQueuePtr events_;

    orcaice::Context context_;
    
    orca::TracerPrx tracerPrx_;

    void setRemoteVerbosity( int error, int warn, int info, int debug );

};

} // namespace

#endif
