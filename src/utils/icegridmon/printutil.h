#ifndef ICEGRIDMON_PRINTUTIL_H
#define ICEGRIDMON_PRINTUTIL_H

#include <string>
#include <IceGrid/Registry.h>
#include <IceGrid/Observer.h>

namespace icegridmon {


std::string toString( const IceGrid::AdapterInfo &a, int indent=0 );
std::string toString( const IceGrid::ObjectInfo &a, int indent=0 );
std::string toString( const IceGrid::NodeInfo &n, int indent=0 );
std::string toString( const IceGrid::ServerState &s );
std::string toString( const IceGrid::ServerDynamicInfo &n, int indent=0 );
std::string toString( const IceGrid::AdapterDynamicInfo &n, int indent=0 );
std::string toString( const IceGrid::NodeDynamicInfo &n, int indent=0 );
std::string longToDateTime( long time );
std::string toString( const Ice::StringSeq &s, int indent=0 );
std::string toString( const IceGrid::StringStringDict &s, int indent=0 );
std::string toString( const IceGrid::ApplicationDescriptor &a, int indent=0 );
std::string toString( const IceGrid::ApplicationInfo &a, int indent=0 );
std::string toString( const IceGrid::DistributionDescriptor &d, int indent=0 );
std::string toString( const IceGrid::ApplicationUpdateDescriptor &a, int indent=0 );
std::string toString( const IceGrid::ApplicationUpdateInfo &a, int indent=0 );
std::string toString( const IceGrid::RegistryInfo &r, int indent=0 );


template<typename Seq>
std::string toString( const Seq &seq, const std::string &descr, int indent=0 )
{
    std::string ind = "";
    for ( int i=0; i < indent; i++ )
        ind += " ";

    std::stringstream ss;
    for ( uint i=0; i < seq.size(); i++ )
    {
        ss << ind << descr << " " << i << ":" << std::endl << toString(seq[i],indent+2);
        if ( i != seq.size()-1 ) 
            ss << std::endl;
    }
    return ss.str();
}

}

#endif
