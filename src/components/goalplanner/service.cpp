/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include "service.h"
#include "component.h"

using namespace std;
using namespace goalplanner;

extern "C"
{
    //
    // Factory function
    //
    IceBox::Service* create( Ice::CommunicatorPtr communicator )
    {
        return new goalplanner::Service;
    }
}

Service::Service()
{
    component_ = new goalplanner::Component;
}
