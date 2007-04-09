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
/*
namespace orca
{
*/
class Odometry2dConsumerI : virtual public orca::Odometry2dConsumer
{
public:

    virtual void setData(const ::orca::Odometry2dData&,
			 const Ice::Current&);
};
/*
class Odometry2dI : virtual public Odometry2d
{
public:

    virtual ::orca::Odometry2dDataPtr getData(const Ice::Current&) const;

    virtual ::orca::Odometry2dGeometryPtr getGeometry(const Ice::Current&) const;

    virtual void subscribe(const ::orca::Odometry2dConsumerPrx&,
			   const Ice::Current&);

    virtual void unsubscribe(const ::orca::Odometry2dConsumerPrx&,
			     const Ice::Current&);
};

}
*/
#endif
