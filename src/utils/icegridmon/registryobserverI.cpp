#include "registryobserverI.h"
#include <iostream>
#include <sstream>
#include "printutil.h"

using namespace std;

namespace icegridmon {

void 
RegistryObserverI::registryInit( const IceGrid::RegistryInfoSeq &registries,
                                 const Ice::Current& )
{
    std::stringstream ss;
    ss << "RegistryObserverI::registryInit:\n";
    ss << "  registries:\n" << toString<IceGrid::RegistryInfoSeq>(registries,"registry",4);
    context_.tracer()->info( ss.str() );
}

void 
RegistryObserverI::registryUp( const IceGrid::RegistryInfo &info,
                               const Ice::Current& )
{
    stringstream ss;
    ss << "RegistryObserverI::registryUp:\n"
       << "  info:\n" << toString(info,4);
    context_.tracer()->info( ss.str() );
}

void 
RegistryObserverI::registryDown( const std::string &name,		      
                                 const Ice::Current& )
{
    stringstream ss;
    ss << "RegistryObserverI::registryDown:\n"
       << "  name: " << name;
    context_.tracer()->info( ss.str() );
}


}
