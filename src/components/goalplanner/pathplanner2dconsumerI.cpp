/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>

#include "pathplanner2dconsumerI.h"

using namespace std;
using namespace orca;
using namespace goalplanner;

PathPlanner2dConsumerI::PathPlanner2dConsumerI(orcaice::PtrProxy<orca::PathPlanner2dDataPtr> &pathBuffer)
    : pathBuffer_(pathBuffer)
{
}

void
PathPlanner2dConsumerI::setData(const ::orca::PathPlanner2dDataPtr& data, const ::Ice::Current&)
{
    cout << "INFO(pathplanner2dconsumerI.cpp): Received results: " << endl;
    cout << orcaice::toVerboseString(data) << endl;
    
    // put them in the buffer now
    pathBuffer_.set( data );
    
}



