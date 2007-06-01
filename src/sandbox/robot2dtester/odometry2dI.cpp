/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#include "odometry2dI.h"

#include <orcaice/orcaice.h>

using namespace std;

void
Odometry2dConsumerI::setData(const ::orca::Odometry2dData& obj,
				   const Ice::Current& current)
{
    cout<<"sub :"<<orcaice::toString(obj)<<endl;
}
