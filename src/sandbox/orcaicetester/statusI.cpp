/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include "statusI.h"

void
StatusConsumerI::setData(const ::orca::StatusData& obj,
				 const Ice::Current& current)
{
}
/*
::orca::StatusDataPtr
orca::StatusI::getData(const Ice::Current& current) const
{
    return 0;
}

void
orca::StatusI::subscribe(const ::orca::StatusConsumerPrx& subscriber,
			 const Ice::Current& current)
{
}

void
orca::StatusI::unsubscribe(const ::orca::StatusConsumerPrx& subscriber,
			   const Ice::Current& current)
{
}
*/
