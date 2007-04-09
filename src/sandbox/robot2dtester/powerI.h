/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef __powerI_h__
#define __powerI_h__

#include <orca/power.h>

/*
namespace orca
{

class PowerI : virtual public Power
{
public:

    virtual ::orca::PowerData getData(const Ice::Current&) const;
};
*/
class PowerConsumerI : virtual public orca::PowerConsumer
{
public:

    virtual void setData(const ::orca::PowerData&,
			 const Ice::Current&);
};
/*
}
*/
#endif
