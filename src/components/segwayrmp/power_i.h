/*
 *  Orca Project: Components for robotics.
 *
 *  Copyright (C) 2004-2006
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef ORCA2_SEGWAY_RMP_POWER_I_H
#define ORCA2_SEGWAY_RMP_POWER_I_H

// include defnition of Ice runtime
#include <Ice/Ice.h>
#include <IceStorm/IceStorm.h>

// include provided interfaces
#include <orca/power.h>

// utilities
#include <orcaice/ptrbuffer.h>


class PowerI : public orca::Power
{
public:
    PowerI( orcaice::PtrBuffer<orca::PowerDataPtr> &power,
                 const IceStorm::TopicPrx &topic );

    virtual ::orca::PowerDataPtr getData(const ::Ice::Current& ) const;

    virtual void subscribe(const ::orca::PowerConsumerPrx&, const ::Ice::Current& = ::Ice::Current());

    virtual void unsubscribe(const ::orca::PowerConsumerPrx&, const ::Ice::Current& = ::Ice::Current());

private:
    // the driver will put the latest data into this proxy
    orcaice::PtrBuffer<orca::PowerDataPtr> &powerProxy_;

    // IceStorm topic to which we send our updates and cand subscribe other to
    IceStorm::TopicPrx topic_;
};

#endif
