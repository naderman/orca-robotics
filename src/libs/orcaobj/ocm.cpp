/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <sstream>
#include <iomanip>
#include "ocm.h"

using namespace std;

namespace orcaobj
{

std::string toString( const orca::FQComponentName& name )
{
    return name.platform + "/" + name.component;
}

std::string toString( const orca::FQInterfaceName& name )
{
    return name.iface + "@" + name.platform + "/" + name.component;
}

std::string toString( const orca::FQTopicName& name )
{
    return name.iface + "/" + name.topic + "@" + name.platform + "/" + name.component;
}

std::string toString( const orca::FQExecutableName& name )
{
    return name.executable + "@" + name.host ;
}

orca::FQComponentName toComponentName( const std::string& s )
{
    orca::FQComponentName fqCName;

    int posSlash = s.find( '/', 0 );
    //cout<<"/ found :"<<posSlash<<endl;

    if ( posSlash<0 ) { 
        // delimeter not found: return null structure
    }
    else {
        fqCName.platform = s.substr( 0, posSlash );
        fqCName.component = s.substr( posSlash+1, s.npos );
    }

    //cout<<"debug: "<<toString(fqCName)<<endl;
    return fqCName;
}

orca::FQInterfaceName toInterfaceName( const std::string& s )
{
    orca::FQInterfaceName fqIName;

    int posAt = s.find( '@', 0 );
    //cout<<"@ found :"<<posAt<<endl;
    int posSlash = s.find( '/', 0 );
    //cout<<"/ found :"<<posSlash<<endl;

    if ( posAt<0 || posSlash<0 ) { 
        // delimeters not found: return null structure
    }
    else {
        fqIName.iface = s.substr( 0, posAt );
        fqIName.platform = s.substr( posAt+1, posSlash-posAt-1 );
        fqIName.component = s.substr( posSlash+1, s.npos );
    }

    //cout<<"debug: "<<toString(fqIName)<<endl;
    return fqIName;
}

std::string toHomeIdentity( const orca::FQComponentName & fqCName )
{
    return "orca." + fqCName.platform + "." + fqCName.component + "/Home";
}

orca::FQTopicName toStatusTopic( const orca::FQComponentName& fqCName )
{
    orca::FQTopicName fqTName;
        
    fqTName.platform  = fqCName.platform;
    fqTName.component = fqCName.component;
    fqTName.iface     = "status";
    fqTName.topic     = "*";

    return fqTName;
}

orca::FQTopicName toTracerTopic( const orca::FQComponentName& fqCName )
{
    orca::FQTopicName fqTName;
        
    fqTName.platform  = fqCName.platform;
    fqTName.component = fqCName.component;
    fqTName.iface     = "tracer";
    fqTName.topic     = "*";

    return fqTName;
}

} // namespace
