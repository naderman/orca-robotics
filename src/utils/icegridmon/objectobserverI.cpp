#include "objectobserverI.h"
#include <iostream>
#include <sstream>
#include <orcaicegrid/printutil.h>

using namespace std;
using namespace orcaicegrid;

namespace icegridmon {

void 
ObjectObserverI::objectInit( const IceGrid::ObjectInfoSeq &objects,
                               const Ice::Current& )
{
    stringstream ss;
    ss << "ObjectObserverI::objectInit:";
    ss << "  objects:\n" << toString(objects,"object",4);
    context_.tracer()->info( ss.str() );
}

void 
ObjectObserverI::objectAdded( const IceGrid::ObjectInfo &info,
                              const Ice::Current& )
{
    stringstream ss;
    ss << "ObjectObserverI::objectAdded:\n"
       << toString(info,2);
    context_.tracer()->info( ss.str() );
}

void 
ObjectObserverI::objectRemoved( const Ice::Identity &id,
                                const Ice::Current& )
{
    stringstream ss;
    ss << "ObjectObserverI::objectRemoved\n"
       << "  id: " << endl
       << "    name:     " << id.name << endl
       << "    category: " << id.category;
    context_.tracer()->info( ss.str() );
}

void 
ObjectObserverI::objectUpdated( const IceGrid::ObjectInfo &info,
                                const Ice::Current& )
{
    stringstream ss;
    ss << "ObjectObserverI::objectUpdated\n"
       << toString(info,2);
    context_.tracer()->info( ss.str() );
}

}
