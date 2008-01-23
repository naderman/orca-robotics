/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include "adapterobserverI.h"
#include <iostream>
#include <sstream>
#include <orcaicegrid/printutil.h>

using namespace std;
using namespace orcaicegrid;

namespace icegridmon {

void 
AdapterObserverI::adapterInit( const IceGrid::AdapterInfoSeq &adpts,
                               const Ice::Current& )
{
    stringstream ss;
    ss << "AdapterObserverI::adapterInit:" << endl
       << "  adapters: " << toString<IceGrid::AdapterInfoSeq>(adpts,"adapter",4);
    context_.tracer().info( ss.str() );
}

void 
AdapterObserverI::adapterAdded( const IceGrid::AdapterInfo &info,
                                const Ice::Current& )
{
    stringstream ss;
    ss << "AdapterObserverI::adapterAdded:\n"
       << toString(info,2);
    context_.tracer().info( ss.str() );
}

void 
AdapterObserverI::adapterRemoved( const std::string &id,
                                  const Ice::Current& )
{
    stringstream ss;
    ss << "AdapterObserverI::adapterRemoved\n"
       << "  id: " << id;
    context_.tracer().info( ss.str() );
}

void 
AdapterObserverI::adapterUpdated( const IceGrid::AdapterInfo &info,
                                  const Ice::Current& )
{
    stringstream ss;
    ss << "AdapterObserverI::adapterUpdated\n"
       << toString(info,2);
    context_.tracer().info( ss.str() );
}

}
