/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>

#include "odometry2dconsumerI.h"

using namespace std;

Odometry2dConsumerI::Odometry2dConsumerI( orcaice::Buffer<orca::Odometry2dData> &pipe )
    : pipe_(pipe)
{
}

void Odometry2dConsumerI::setData(const orca::Odometry2dData& data, const Ice::Current&)
{
    pipe_.push( data );
}
