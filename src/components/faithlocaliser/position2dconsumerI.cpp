/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>

#include "position2dconsumerI.h"

using namespace std;

Position2dConsumerI::Position2dConsumerI( orcaice::Buffer<orca::Position2dData> & pipe )
    : pipe_(pipe)
{
}

void Position2dConsumerI::setData(const orca::Position2dData& data, const Ice::Current&)
{
//     cout<<"pub : "<<data<<endl;
    pipe_.push( data );
}
