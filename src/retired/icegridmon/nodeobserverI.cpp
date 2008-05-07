/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include "nodeobserverI.h"
#include <iostream>
#include <sstream>
#include <orcaicegrid/printutil.h>

using namespace std;
using namespace orcaicegrid;

namespace icegridmon {

void 
NodeObserverI::nodeInit( const IceGrid::NodeDynamicInfoSeq &nodes,
                         const Ice::Current& )
{
    stringstream ss;
    ss << "NodeObserverI::nodeInit:\n"
       << "  nodes:\n"
       << toString(nodes,"node",4);
    context_.tracer().info( ss.str() );
}

void 
NodeObserverI::nodeUp( const IceGrid::NodeDynamicInfo &info,
                       const Ice::Current& )
{
    stringstream ss;
    ss << "NodeObserverI::nodeUp:\n" << toString(info,2);
    context_.tracer().info( ss.str() );
}

void 
NodeObserverI::nodeDown( const std::string &name,
                         const Ice::Current& )
{
    stringstream ss;
    ss << "NodeObserverI::nodeDown: " << name;
    context_.tracer().info( ss.str() );
}

void 
NodeObserverI::updateServer( const std::string &node, 
                             const IceGrid::ServerDynamicInfo &updatedInfo,
                             const Ice::Current& )
{
    stringstream ss;
    ss << "NodeObserverI::updateServer:\n"
       << "  node: " << node << endl
       << "  updatedInfo: " << endl << toString(updatedInfo,4);
    context_.tracer().info( ss.str() );
}

void 
NodeObserverI::updateAdapter( const std::string &node,
                              const IceGrid::AdapterDynamicInfo &updatedInfo,
                              const Ice::Current& )
{
    stringstream ss;
    ss << "NodeObserverI::updateAdapter:\n" 
       << "  node: " << node << endl
       << "  updatedInfo: " << endl << toString(updatedInfo,4);
    context_.tracer().info( ss.str() );
}

void 
NodeObserverI::nodeUpdated( const IceGrid::NodeInfo &info,
                            const Ice::Current& )
{
    stringstream ss;
    ss << "NodeObserverI::nodeUpdated:\n"
       << "  nodeInfo: " << endl << toString(info,4);
    context_.tracer().info( ss.str() );
}                             

}
