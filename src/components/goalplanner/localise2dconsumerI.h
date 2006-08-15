/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef _GOALPLANNER_LOCALISE_CONSUMER_I_H_
#define _GOALPLANNER_LOCALISE_CONSUMER_I_H_

#include <iostream>

#include <orca/localise2d.h>
#include <orcaice/ptrproxy.h>

class Localise2dConsumerI : public orca::Localise2dConsumer
{
    
public:
    Localise2dConsumerI(orcaice::PtrProxy<orca::Localise2dDataPtr> &localise2dDataBuffer);
    virtual void setData(const orca::Localise2dDataPtr& data, const Ice::Current&);

private:
    orcaice::PtrProxy<orca::Localise2dDataPtr> &localise2dDataBuffer_;
};

#endif
