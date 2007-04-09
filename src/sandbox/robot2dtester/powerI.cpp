/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#include "powerI.h"

#include <orcaice/orcaice.h>

using namespace std;

/*
::orca::PowerData
orca::PowerI::getData(const Ice::Current& current) const
{
    return 0;
}
*/

void
PowerConsumerI::setData(const ::orca::PowerData& obj,
			      const Ice::Current& current)
{
    cout<<"sub :"<<orcaice::toString(obj)<<endl;
}
