/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include "service.h"
#include "component.h"

using namespace std;

extern "C"
{
    // Factory function

    IceBox::Service* create( Ice::CommunicatorPtr communicator )
    {
        return new wifi::Service;
    }
}

namespace wifi {

Service::Service()
{
    component_ = new Component;
}

}