/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_TRACERMON_TRACER_CONSUMER_I_H
#define ORCA2_TRACERMON_TRACER_CONSUMER_I_H

// #include <iostream>

#include <orca/tracer.h>
// #include <orcaice/orcaice.h>
#include "user.h"

namespace tracermon
{

class TracerConsumerI : public orca::TracerConsumer
{
public:
    TracerConsumerI( User& user ) :
        user_(user) {};

    virtual void setData(const orca::TracerData& data, const Ice::Current&)
    {
//         std::cout << orcaice::toString( data ) << std::endl;
        user_.newTraceMessage( data );
    }

private:
    User& user_;
};

} // namespace

#endif
