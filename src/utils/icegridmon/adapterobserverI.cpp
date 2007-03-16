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
    ss << "AdapterObserverI::adapterInit:" << endl
       << "  adapters: " << toString<IceGrid::AdapterInfoSeq>(adpts,"adapter",4);
    context_.tracer()->info( ss.str() );
}

void 
AdapterObserverI::adapterAdded( const IceGrid::AdapterInfo &info,
                                const Ice::Current& )
{
    stringstream ss;
    ss << "AdapterObserverI::adapterAdded:\n"
       << toString(info,2);
    context_.tracer()->info( ss.str() );
}

void 
AdapterObserverI::adapterRemoved( const std::string &id,
                                  const Ice::Current& )
{
    stringstream ss;
    ss << "AdapterObserverI::adapterRemoved\n"
       << "  id: " << id;
    context_.tracer()->info( ss.str() );
}

void 
AdapterObserverI::adapterUpdated( const IceGrid::AdapterInfo &info,
                                  const Ice::Current& )
{
    stringstream ss;
    ss << "AdapterObserverI::adapterUpdated\n"
       << toString(info,2);
    context_.tracer()->info( ss.str() );
}

}
