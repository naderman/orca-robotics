/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef TRACER_CONSUMER_I_H
#define TRACER_CONSUMER_I_H

#include <iostream>
#include <orca/tracer.h>
#include <orcaice/orcaice.h>

namespace sysmon {

class TracerConsumerI : public orca::TracerConsumer
{
    public:
        virtual void setData(const orca::TracerData& data, const Ice::Current&)
        {
            std::cout << orcaice::toString(data) << std::endl;
        }
};

}

#endif
