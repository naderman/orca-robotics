/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef _GOALPLANNER_LOCALISE_CONSUMER_I_H_
#define _GOALPLANNER_LOCALISE_CONSUMER_I_H_

#include <iostream>

#include <orca/localise2d.h>
#include <orcaice/proxy.h>

class Localise2dConsumerI : public orca::Localise2dConsumer
{
    
public:
    Localise2dConsumerI(orcaice::Proxy<orca::Localise2dData>& localise2dDataPipe);
    virtual void setData(const orca::Localise2dData& data, const Ice::Current&);

private:
    orcaice::Proxy<orca::Localise2dData>& localise2dDataPipe_;
};

#endif
