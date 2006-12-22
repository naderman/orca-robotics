/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2006-2007 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>

#include <orcaice/orcaice.h>
#include <orcalog/orcalog.h>
#include "component.h"

using namespace std;
using namespace orca;
using namespace orcaice;

namespace icegridmon {

Component::Component( const std::string & compName )
    : orcaice::Component( compName )
{
}

Component::~Component()
{
}

void 
Component::start()
{
    // config file parameters
    Ice::PropertiesPtr props = properties();
    std::string prefix = tag() + ".Config.";
   
    // activate component, this may throw and it will kill us
    activate();

}

void Component::stop()
{
//     context().tracer()->debug("Stopping logger component", 5 );
    // there are no threads to stop, we are done
}

} // namespace
