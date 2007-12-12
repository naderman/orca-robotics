/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_TRACERMON_NETWORK_HANDLER_H
#define ORCA2_TRACERMON_NETWORK_HANDLER_H

#include <orca/tracer.h>
#include <hydroutil/safethread.h>
#include <orcaice/context.h>
#include <hydroutil/eventqueue.h>
#include "user.h"
#include "network.h"

namespace tracermon
{

// class MainThread : public Network
class MainThread : public hydroutil::SafeThread, public Network //, virtual public orca::TracerConsumer
{
public:

    MainThread( User* user,
                    const orcaice::Context & context );
    virtual ~MainThread();

    // from thread
    virtual void walk();
    
    // from Network
    virtual void setVerbosityLevel( int error, int warn, int info, int debug );

    // from consumer
//     virtual void setData(const orca::TracerData& data, const Ice::Current&);

private:

    User* user_;
    hydroutil::EventQueuePtr events_;

    orcaice::Context context_;
    
    orca::TracerPrx tracerPrx_;

    void setRemoteVerbosity( int error, int warn, int info, int debug );

};

} // namespace

#endif
