#include "printutil.h"
#include <iostream>

using namespace std;

namespace orcaicegrid {

inline std::string makeIndent( int indent )
{
    string ind = "";
    for ( int i=0; i < indent; i++ )
        ind += " ";
    return ind;
}

std::string toString( const IceGrid::RegistryInfo &r, int indent )
{
    string ind = makeIndent(indent);

    stringstream ss;
    ss << ind << "name:     " << r.name << endl
       << ind << "hostname: " << r.hostname;
    return ss.str();
}

std::string toString( const IceGrid::AdapterInfo &a, int indent )
{
    string ind = makeIndent(indent);

    stringstream ss;
    ss << ind << "id: " << a.id << endl
       << ind << "proxy: " << a.proxy << endl
       << ind << "replicaGroupId: " << a.replicaGroupId;
    return ss.str();
}

std::string toString( const IceGrid::ObjectInfo &a, int indent )
{
    string ind = makeIndent(indent);

    stringstream ss;
    ss << ind << "proxy: " << a.proxy << endl
       << ind << "type:  " << a.type;
    return ss.str();
}

std::string toString( const IceGrid::NodeInfo &n, int indent )
{
    string ind = makeIndent(indent);

    stringstream ss;
    ss << ind << "name:        " << n.name << endl
       << ind << "os:          " << n.os << endl
       << ind << "hostname:    " << n.hostname << endl
       << ind << "release:     " << n.release << endl
       << ind << "version:     " << n.version << endl
       << ind << "machine:     " << n.machine << endl
       << ind << "nProcessors: " << n.nProcessors << endl
       << ind << "dataDir:     " << n.dataDir;
    return ss.str();
}

std::string toString( const IceGrid::ServerState &s )
{
    if ( s == IceGrid::Inactive )
        return "Inactive";
    else if ( s == IceGrid::Activating )
        return "Activating";

    else if ( s == IceGrid::ActivationTimedOut )
        return "ActivationTimedOut";
    else if ( s == IceGrid::Active )
        return "Active";
    else if ( s == IceGrid::Deactivating )
        return "Deactivating";
    else if ( s == IceGrid::Destroying )
        return "Destroying";
    else if ( s == IceGrid::Destroyed )
        return "Destroyed";
    else 
        return "??Unknown??";
}

std::string toString( const IceGrid::ServerDynamicInfo &n, int indent )
{
    string ind = makeIndent(indent);

    stringstream ss;
    ss << ind << "id:      " << n.id << endl
       << ind << "state:   " << toString(n.state) << endl
       << ind << "pid:     " << n.pid << endl
       << ind << "enabled: " << n.enabled;    
    return ss.str();
}

std::string toString( const IceGrid::AdapterDynamicInfo &n, int indent )
{
    string ind = makeIndent(indent);

    stringstream ss;
    ss << ind << "id:    " << n.id << endl
       << ind << "proxy: " << n.proxy;
    return ss.str();
}

std::string toString( const IceGrid::NodeDynamicInfo &n, int indent )
{
    string ind = makeIndent(indent);

    stringstream ss;
    ss << ind << "info: " << endl << toString(n.info,indent+2) << endl
       << ind << "servers: " << endl
       << toString<IceGrid::ServerDynamicInfoSeq>(n.servers,"server",indent+2) << endl;
    ss << ind << "adapters: " << endl
       << toString<IceGrid::AdapterDynamicInfoSeq>(n.adapters,"adapter",indent+2);
    return ss.str();
}

std::string longToDateTime( long time )
{
    IceUtil::Time t = IceUtil::Time::seconds(time);
    return t.toDateTime();
}

std::string toString( const Ice::StringSeq &s, int indent )
{
    string ind = makeIndent(indent);

    stringstream ss;
    for ( uint i=0; i < s.size(); i++ )
    {
        ss << ind << i << ": " << s[i] << endl;
    }
    return ss.str();
}

std::string toString( const IceGrid::StringStringDict &s, int indent )
{
    string ind = makeIndent(indent);

    stringstream ss;
    for ( IceGrid::StringStringDict::const_iterator i = s.begin();
          i != s.end();
          i++ )
    {
        ss << ind << i->first << ": " << i->second << endl;
    }
    return ss.str();
}

std::string toString( const IceGrid::ServerDescriptor &s, int indent )
{
    string ind = makeIndent(indent);

    stringstream ss;
    ss << ind << "id:  " << s.id << endl
       << ind << "exe: " << s.exe << endl
       << ind << "(detailed ServerDescriptor info omitted)" << endl;

    return ss.str();
}

std::string toString( const IceGrid::NodeDescriptor &n, int indent )
{
    string ind = makeIndent(indent);

    stringstream ss;
    ss << ind << "description: " << n.description << endl;
    ss << ind << "variables:" << endl;
    for ( std::map<string,string>::const_iterator it = n.variables.begin();
          it != n.variables.end();
          it++ )
    {
        ss << ind << "  " << it->first << ": " << it->second << endl;
    }
    ss << ind << "servers (size "<<n.servers.size()<<"):" << endl;
    for ( uint i=0; i < n.servers.size(); i++ )
    {
        const IceGrid::ServerDescriptorPtr &s = n.servers[i];
        ss << ind << toString( *s, indent+2 ) << endl;
    }
    ss << ind << "(detailed NodeDescriptor info omitted)" << endl;
    return ss.str();
}

std::string toString( const IceGrid::ApplicationDescriptor &a, int indent )
{
    string ind = makeIndent(indent);

    stringstream ss;
    ss << ind << "name:        " << a.name << endl
       << ind << "description: " << a.description << endl
       << ind << "variables: " << endl;
    for ( std::map<string,string>::const_iterator it = a.variables.begin();
          it != a.variables.end();
          it++ )
    {
        ss << ind << "  " << it->first << ": " << it->second << endl;
    }
    ss << ind << "nodes: " << endl;
    for ( std::map<string,IceGrid::NodeDescriptor>::const_iterator it = a.nodes.begin();
          it != a.nodes.end();
          it++ )
    {
        ss << ind << "  " << it->first << ": "<< endl << toString(it->second,indent+2) << endl;
    }
    ss << ind << "(detailed app description omitted)";
    return ss.str();
}
    
std::string toString( const IceGrid::ApplicationInfo &a, int indent )
{
    string ind = makeIndent(indent);

    stringstream ss;
    ss << ind << "uuid:       " << a.uuid << endl
       << ind << "createTime: " << a.createTime << endl
       << ind << "createTime: " << longToDateTime(a.createTime) << endl
       << ind << "createUser: " << a.createUser << endl
       << ind << "updateTime: " << longToDateTime(a.updateTime) << endl
       << ind << "updateUser: " << a.updateUser << endl
       << ind << "revision:   " << a.revision << endl
       << ind << "descriptor: " << endl << toString( a.descriptor, indent+2 );
    return ss.str();
}

std::string toString( const IceGrid::DistributionDescriptor &d, int indent )
{
    string ind = makeIndent(indent);

    stringstream ss;
    ss << ind << "icepatch: " << d.icepatch << endl;
    ss << ind << "directories:"<<endl<<toString(d.directories,indent+2);
    return ss.str();
}

std::string toString( const IceGrid::ApplicationUpdateDescriptor &a, int indent )
{
    string ind = makeIndent(indent);

    stringstream ss;
    ss << ind << "name:        " << a.name << endl;
    if ( a.description != 0 )
        ss << ind << "description: " << a.description->value << endl;
    if ( a.distrib != 0 )
        ss << ind << "distrib:     " << endl << toString(a.distrib->value,indent+2) << endl;
    ss << "variables: " << endl << toString(a.variables,indent+2) << endl
       << "removeVariables: " << endl << toString(a.removeVariables,indent+2) << endl
       << "(lots more detail omitted)" << endl;
    return ss.str();
}

std::string toString( const IceGrid::ApplicationUpdateInfo &a, int indent )
{
    string ind = makeIndent(indent);

    stringstream ss;
    ss << ind << "updateTime: " << longToDateTime(a.updateTime) << endl
       << ind << "updateUser: " << a.updateUser << endl
       << ind << "revision:   " << a.revision << endl
       << ind << "descriptor: " << endl << toString(a.descriptor,indent+2);
    return ss.str();
}

}
