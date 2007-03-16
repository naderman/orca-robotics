#include "objectobserverI.h"
#include <iostream>
#include <sstream>
#include "printutil.h"

using namespace std;

namespace icegridmon {

void 
ObjectObserverI::objectInit( const IceGrid::ObjectInfoSeq &objects,
                               const Ice::Current& )
{
    stringstream ss;
    ss << "objectInit:";
    ss << "  objects:\n" << toString(objects);
    context_.tracer()->info( ss.str() );
}

void 
ObjectObserverI::objectAdded( const IceGrid::ObjectInfo &info,
                              const Ice::Current& )
{
    stringstream ss;
    ss << "objectAdded:\n"
       << toString(info,2);
    context_.tracer()->info( ss.str() );
}

void 
ObjectObserverI::objectRemoved( const Ice::Identity &id,
                                const Ice::Current& )
{
    stringstream ss;
    ss << "objectRemoved\n"
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
    ss << "objectUpdated\n"
       << toString(info,2);
    context_.tracer()->info( ss.str() );
}

}
