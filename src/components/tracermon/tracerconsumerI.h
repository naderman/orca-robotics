/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_TRACERMON_TRACERCONSUMER_I_H
#define ORCA2_TRACERMON_TRACERCONSUMER_I_H

#include <iostream>

#include <orca/tracer.h>
#include <orcaice/orcaice.h>

class TracerConsumerI : public orca::TracerConsumer
{
    public:
        virtual void setData(const orca::TracerData& data, const Ice::Current&)
        {
            std::cout << orcaice::toString( data ) << std::endl;
        }
};

#endif
