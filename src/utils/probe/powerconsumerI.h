/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_POWER_CONSUMER_I_H
#define ORCA_POWER_CONSUMER_I_H

#include <orca/power.h>

#include <iostream>
#include <orcaobj/stringutils.h>

class PowerConsumerI : public orca::PowerConsumer
{
    public:
        virtual void setData(const orca::PowerDataPtr& data, const Ice::Current&)
        {
            std::cout << orcaice::toString(data) << std::endl;
        };

};

#endif
