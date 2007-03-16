#include "nodeobserverI.h"
#include <iostream>
#include <sstream>
#include "printutil.h"

using namespace std;

namespace icegridmon {

void 
NodeObserverI::nodeInit( const IceGrid::NodeDynamicInfoSeq &nodes,
                         const Ice::Current& )
{
    stringstream ss;
    ss << "nodeInit:\n"
       << "  nodes:\n"
       << toString(nodes,4);
    context_.tracer()->info( ss.str() );
}

void 
NodeObserverI::nodeUp( const IceGrid::NodeDynamicInfo &info,
                       const Ice::Current& )
{
    stringstream ss;
    ss << "nodeUp:\n" << toString(info,2);
    context_.tracer()->info( ss.str() );
}

void 
NodeObserverI::nodeDown( const std::string &name,
                         const Ice::Current& )
{
    stringstream ss;
    ss << "nodeDown: " << name;
    context_.tracer()->info( ss.str() );
}

void 
NodeObserverI::updateServer( const std::string &node, 
                             const IceGrid::ServerDynamicInfo &updatedInfo,
                             const Ice::Current& )
{
    stringstream ss;
    ss << "updateServer:\n"
       << "  node: " << node << endl
       << "  updatedInfo: " << endl << toString(updatedInfo,4);
    context_.tracer()->info( ss.str() );
}

void 
NodeObserverI::updateAdapter( const std::string &node,
                              const IceGrid::AdapterDynamicInfo &updatedInfo,
                              const Ice::Current& )
{
    stringstream ss;
    ss << "updateAdapter:\n" 
       << "  node: " << node << endl
       << "  updatedInfo: " << endl << toString(updatedInfo,4);
    context_.tracer()->info( ss.str() );
}

void 
NodeObserverI::nodeUpdated( const IceGrid::NodeInfo &info,
                            const Ice::Current& )
{
    stringstream ss;
    ss << "nodeUpdated:\n"
       << "  nodeInfo: " << endl << toString(info,4);
    context_.tracer()->info( ss.str() );
}                             

}
