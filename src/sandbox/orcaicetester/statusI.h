/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef __statusI_h__
#define __statusI_h__

#include <orca/status.h>

//namespace orca
//{

class StatusConsumerI : virtual public orca::StatusConsumer
{
public:

    virtual void setData(const ::orca::StatusData&,
        	   const Ice::Current&);
};
/*
class StatusI : virtual public Status
{
public:

    virtual ::orca::StatusDataPtr getData(const Ice::Current&) const;

    virtual void subscribe(const ::orca::StatusConsumerPrx&,
			   const Ice::Current&);

    virtual void unsubscribe(const ::orca::StatusConsumerPrx&,
			     const Ice::Current&);
};
*/
//}

#endif
