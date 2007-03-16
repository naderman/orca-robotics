#include "adapterobserverI.h"
#include <iostream>
#include <sstream>
#include "printutil.h"

using namespace std;

namespace icegridmon {

void 
AdapterObserverI::adapterInit( const IceGrid::AdapterInfoSeq &adpts,
                               const Ice::Current& )
{
    stringstream ss;
    ss << "adapterInit:" << endl
       << "  adapters: " << toString<IceGrid::AdapterInfoSeq>(adpts,4);
    context_.tracer()->info( ss.str() );
}

void 
AdapterObserverI::adapterAdded( const IceGrid::AdapterInfo &info,
                                const Ice::Current& )
{
    stringstream ss;
    ss << "adapterAdded:\n"
       << toString(info,2);
    context_.tracer()->info( ss.str() );
}

void 
AdapterObserverI::adapterRemoved( const std::string &id,
                                  const Ice::Current& )
{
    stringstream ss;
    ss << "adapterRemoved\n"
       << "  id: " << id;
    context_.tracer()->info( ss.str() );
}

void 
AdapterObserverI::adapterUpdated( const IceGrid::AdapterInfo &info,
                                  const Ice::Current& )
{
    stringstream ss;
    ss << "adapterUpdated\n"
       << toString(info,2);
    context_.tracer()->info( ss.str() );
}

}
