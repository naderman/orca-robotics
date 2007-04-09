/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_POS_CONSUMER_I_H
#define ORCA2_POS_CONSUMER_I_H

#include <iostream>

#include "platform2dI.h"
#include <orcaice/orcaice.h>

namespace noisyrobot2d {

class Position2dConsumerI : public orca::Position2dConsumer
{
public:
    
    Position2dConsumerI( Platform2dI &platform2dI )
        : platform2dI_(platform2dI)
        {}

    virtual void setData(const orca::Position2dData& data, const Ice::Current&)
    {
        platform2dI_.localSetOdom( data );
    }

    private:

    Platform2dI &platform2dI_;
};

}

#endif

