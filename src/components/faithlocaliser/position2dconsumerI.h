/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_POSITION2D_CONSUMER_I_H
#define ORCA_POSITION2D_CONSUMER_I_H

// include defnition of Ice runtime
#include <Ice/Ice.h>
#include <orcaice/ptrbuffer.h>
#include <orca/position2d.h>

class Position2dConsumerI : public orca::Position2dConsumer
{
    public:
        Position2dConsumerI( orcaice::Buffer<orca::Position2dData> & pipe );
        virtual void setData(const orca::Position2dData& data, const Ice::Current&);

    private:
        orcaice::Buffer<orca::Position2dData> & pipe_;
};

#endif
