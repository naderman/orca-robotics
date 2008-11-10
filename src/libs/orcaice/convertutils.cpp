/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <string>

#include <orcaice/convertutils.h>
// #include <hydroutil/sysutils.h>
// #include <orcaice/proputils.h>
// #include <orcaice/printutils.h>
#include <orcaice/exceptions.h>
#include <orcaice/stringutils.h>
// 
// #include "detail/privateutils.h"

using namespace std;

namespace orcaice
{

orca::FQComponentName toComponent( const orca::FQInterfaceName& fqIName )
{
    orca::FQComponentName fqCName;
    fqCName.platform = fqIName.platform;
    fqCName.component = fqIName.component;
    return fqCName;
}

orca::FQInterfaceName toInterface( const orca::FQComponentName& fqCName, const std::string &iface )
{
    orca::FQInterfaceName fqIName;
    fqIName.platform = fqCName.platform;
    fqIName.component = fqCName.component;
    fqIName.iface = iface;
    return fqIName;
}

orca::FQTopicName 
toTopic( const orca::FQInterfaceName& fqIName, const std::string& subtopic )
{
    orca::FQTopicName fqTName;
    fqTName.platform = fqIName.platform;
    fqTName.component = fqIName.component;
    fqTName.iface = fqIName.iface;
    fqTName.topic = subtopic;
    return fqTName;
}

orca::FQTopicName
toTopic( const orca::FQComponentName& fqCName, const std::string &iface, const std::string& subtopic )
{
    orca::FQTopicName fqTName;
    fqTName.platform = fqCName.platform;
    fqTName.component = fqCName.component;
    fqTName.iface = iface;
    fqTName.topic = subtopic;
    return fqTName;
}

std::string
toTopicAsString( const orca::FQComponentName& component, const std::string &ifaceName, const std::string& subtopic )
{
    return orcaice::toString( toTopic( component, ifaceName, subtopic ) );
}

orca::FQTopicName 
toStatusTopic( const orca::FQComponentName& fqCName )
{
    orca::FQTopicName fqTName;
        
    fqTName.platform  = fqCName.platform;
    fqTName.component = fqCName.component;
    fqTName.iface     = "status";
    fqTName.topic     = "*";

    return fqTName;
}

orca::FQTopicName 
toTracerTopic( const orca::FQComponentName& fqCName )
{
    orca::FQTopicName fqTName;
        
    fqTName.platform  = fqCName.platform;
    fqTName.component = fqCName.component;
    fqTName.iface     = "tracer";
    fqTName.topic     = "*";

    return fqTName;
}

} // namespace
