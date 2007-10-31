/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>

#include "pathplanner2dconsumerI.h"

using namespace std;
using namespace orca;
using namespace pathplantester;

PathPlanner2dConsumerI::PathPlanner2dConsumerI()
{
}

void
PathPlanner2dConsumerI::setData(const ::orca::PathPlanner2dData& data, const ::Ice::Current&)
{
    cout << "INFO(pathplanner2dconsumerI.cpp): Received results: " << endl;
    cout << orcaice::toVerboseString(data) << endl;
}



