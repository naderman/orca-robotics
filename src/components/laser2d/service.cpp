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

extern "C"
{
    // Factory function

    IceBox::Service* create( Ice::CommunicatorPtr communicator )
    {
        return new sicklaser::ServiceI;
    }
}

namespace sicklaser {

ServiceI::ServiceI()
{
    component_ = new Component;
}

}
