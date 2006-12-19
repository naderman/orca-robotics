/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <string>

#include <orcaice/configutils.h>
#include <orcaice/sysutils.h>
#include <orcaice/proputils.h>
#include <orcaice/printutils.h>
#include <orcaice/exceptions.h>

#include <orcaobj/stringutils.h>

#include "privateutils.h"

using namespace std;

namespace orcaice
{

// NOTE: utility function, not part of public interface
std::string
getProvidedName( const Context & context, const std::string &ifaceTag )
{
    std::string ifaceName;
    orcaice::getProperty( context.properties(),
                              context.tag()+".Provides."+ifaceTag+".Name", ifaceName );
    if( ifaceName.empty() )
    {
context.tracer()->print("getting getProvidedName for tag="+ifaceTag);
cout<<"getting getProvidedName for tag="+ifaceTag<<endl;
        std::string errorString = context.tag()+": "
            + warnMissingProperty("provided interface name for tag '" + ifaceTag + "'",
                                   context.tag()+".Provides."+ifaceTag+".Name");
        initTracerError( context, errorString, 2 );
        throw orcaice::ConfigFileException(ERROR_INFO,errorString);
    }

    return ifaceName;
}

// NOTE: utility function, not part of public interface
// example: prefix="Something.Something."
std::vector<std::string>
getFieldsForPrefix( const Context & context, const std::string & prefix )
{
    int prefixLength = prefix.size();
    
    Ice::PropertyDict props =
        context.properties()->getPropertiesForPrefix( prefix );
        
    std::vector<std::string> fields;
    std::string field;
    
    for ( Ice::PropertyDict::iterator i=props.begin(); i!=props.end(); ++i ) {
        int posDot = i->first.find( '.', prefixLength+1 );
        field = i->first.substr( prefixLength, posDot-prefixLength );
        
        fields.push_back( field );
//         cout<<i->first<<"\t"<<field<<endl;
    }
    return fields;
}

orca::FQInterfaceName
getProvidedInterface( const Context & context, const std::string &ifaceTag )
{
    if ( ifaceTag.empty() ) {
        throw orcaice::ConfigFileException(ERROR_INFO, "Empty interface tag");
    }
    context.tracer()->print("getting getProvidedInterface for tag="+ifaceTag);
    orca::FQInterfaceName fqIName;
    fqIName.platform = context.name().platform;
    fqIName.component = context.name().component;
    fqIName.iface = getProvidedName(context,ifaceTag);

    return fqIName;
}

orca::FQTopicName
getProvidedTopic( const Context & context, const std::string &ifaceTag, const std::string & subtopic )
{
    if ( ifaceTag.empty() ) {
        throw orcaice::ConfigFileException(ERROR_INFO, "Empty interface tag");
    }

    orca::FQTopicName fqTName;
    fqTName.platform = context.name().platform;
    fqTName.component = context.name().component;
    fqTName.iface = getProvidedName( context, ifaceTag );
    fqTName.topic = subtopic;

    initTracerPrint( context, "will publish to topic '"+orcaice::toString(fqTName)+"'",2 );
    return fqTName;
}

std::string
getRequiredInterfaceAsString( const Context & context, const std::string &ifaceTag )
{
    if ( ifaceTag.empty() ) {
        throw orcaice::ConfigFileException(ERROR_INFO, "Empty interface tag");
    }
    std::string prefix = context.tag()+".Requires."+ifaceTag+".";

    string proxy;
    if ( orcaice::getProperty( context.properties(), prefix+"Proxy", proxy ) ) {
        std::string errorString = context.tag()+": "
            + warnMissingProperty("required interface proxy for tag '" + ifaceTag + "'",
                                   context.tag()+".Requires."+ifaceTag+".Proxy");
        initTracerError( context, errorString, 2 );
        throw orcaice::ConfigFileException(ERROR_INFO,errorString);
    }

    // for indirect proxies only: if platform is set to 'local', replace it by hostname
    orca::FQInterfaceName fqname = orcaice::toInterfaceName( proxy );
    if ( !fqname.iface.empty() && !fqname.platform.empty() && !fqname.component.empty() ) {
        if ( fqname.platform=="local" ) {
            fqname.platform = getHostname();
            initTracerWarning( context, "set remote platform name to hostname="+fqname.platform, 2 );
        }
        proxy = orcaice::toString( fqname );
    }

    return proxy;
}

std::vector<std::string>
getProvidedTags( const Context & context )
{
    std::string prefix = context.tag()+".Provides.";
    return getFieldsForPrefix( context, prefix );
}

std::vector<std::string>
getRequiredTags( const Context & context )
{
    std::string prefix = context.tag()+".Requires.";
    return getFieldsForPrefix( context, prefix );
}

// THESE FUNCTIONS WERE OBSOLETED TO REDUCE THE SIZE OF THE API

/*
orca::FQTopicName
getRequiredTopic( const Context & context, const std::string &ifaceTag )
{
    orca::FQInterfaceName fqIName = getRequiredInterface( context, ifaceTag );

    orca::FQTopicName fqTName;
    fqTName.platform = fqIName.platform;
    fqTName.component = fqIName.component;
    fqTName.iface = fqIName.iface;
    fqTName.topic = "*";

    initTracerPrint( context, "will subscribe to topic '"+orcaice::toString( fqTName )+"'",2 );
    return fqTName;
}

Ice::ObjectPrx
getRequiredInterfaceAsProxy( const Context & context, const std::string &ifaceTag )
{
    return context.communicator()->stringToProxy( getRequiredInterfaceAsString(context,ifaceTag) );
}
Ice::Identity
getProvidedNameAsIdentity( const Context & context, const std::string &ifaceTag )
{
    return Ice::stringToIdentity( getProvidedName(context,ifaceTag) );
}
std::string getProvidedTopicAsString( const Context & context, const std::string &ifaceTag )
{
    return toString( getProvidedTopic(context,ifaceTag) );
}
std::string getRequiredInterfaceAsString( const Context & context, const std::string &ifaceTag )
{
    return toString( getRequiredInterface(context,ifaceTag) );
}
std::string getRequiredTopicAsString( const Context & context, const std::string &ifaceTag )
{
    return toString( getRequiredTopic(context,ifaceTag ) );
}
*/

} // namespace
