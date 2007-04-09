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
/*
::orca::Odometry2dDataPtr
orca::Odometry2dI::getData(const Ice::Current& current) const
{
    return 0;
}

::orca::Odometry2dGeometryPtr
orca::Odometry2dI::getGeometry(const Ice::Current& current) const
{
    return 0;
}

void
orca::Odometry2dI::subscribe(const ::orca::Odometry2dConsumerPrx& subscriber,
			     const Ice::Current& current)
{
}

void
orca::Odometry2dI::unsubscribe(const ::orca::Odometry2dConsumerPrx& subscriber,
			       const Ice::Current& current)
{
}
*/
