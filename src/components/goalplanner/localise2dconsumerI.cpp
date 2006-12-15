/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
 
#include <orcaice/orcaice.h>
#include "localise2dconsumerI.h"

using namespace std;

Localise2dConsumerI::Localise2dConsumerI(orcaice::PtrProxy<orca::Localise2dDataPtr> &localise2dDataBuffer)
    :localise2dDataBuffer_(localise2dDataBuffer)
{
}

void
Localise2dConsumerI::setData(const orca::Localise2dDataPtr& data, const Ice::Current&)
{
    //cout << "TRACE(localise2dconsumerI.cpp): Just received new localise2d data" << endl;
    localise2dDataBuffer_.set(data);
}
