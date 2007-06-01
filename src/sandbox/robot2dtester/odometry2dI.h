/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef __odometry2dI_h__
#define __odometry2dI_h__

#include <orca/odometry2d.h>

class Odometry2dConsumerI : virtual public orca::Odometry2dConsumer
{
public:

    virtual void setData(const ::orca::Odometry2dData&,
			 const Ice::Current&);
};
#endif
