/*
 * Orca-Robotics Project: Components for robotics
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <string>

#include <orcaice/configutils.h>
#include <hydroutil/sysutils.h>
#include <orcaice/proputils.h>
#include <orcaice/printutils.h>
#include <orcaice/exceptions.h>
#include <orcaice/stringutils.h>
#include <orcaice/convertutils.h>

#include "detail/privateutils.h"

using namespace std;

namespace orcaice
{

namespace {

// removes entries from the vector of strings if they don't start from the
// pattern specified.
void filter_start( std::vector<std::string>& s, const std::string& pattern )
{
    for ( std::vector<std::string>::iterator it=s.begin(); it<s.end(); ) {
//         cout<<"DEBUG: comparing tag="<<*it<<" with pattern="<<pattern<<endl;
        // pattern may not be longer than the tag itself
        if ( pattern.size()>it->size() || pattern != it->substr(0,pattern.size()) ) {
//             cout<<"DEBUG: erasing "<<*it<<endl;
            s.erase(it);
        }
        else {
            ++it;
        }
    }
}

// NOTE: utility function, not part of public interface
std::string
getProvidedName( const Context& context, const std::string &ifaceTag )
{
    std::string ifaceName;
    orcaice::getProperty( context.properties(), context.tag()+".Provides."+ifaceTag+".Name", ifaceName );
    if( ifaceName.empty() )
    {
//         context.tracer().debug("getting getProvidedName for tag="+ifaceTag);
        std::string errorString = context.tag()+": "
            + warnMissingProperty("provided interface name for tag '" + ifaceTag + "'",
                                   context.tag()+".Provides."+ifaceTag+".Name");
        initTracerError( context, errorString );
        throw orcaice::ConfigFileException(ERROR_INFO,errorString);
    }

    return ifaceName;
}

// NOTE: utility function, not part of public interface
// example: prefix="Something.Something."
std::vector<std::string>
getFieldsForPrefix( const Context& context, const std::string& prefix )
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

}

//////////////////////////////////////////////////////

orca::FQInterfaceName
getProvidedInterface( const Context& context, const std::string &ifaceTag )
{
    if ( ifaceTag.empty() ) {
        throw orcaice::ConfigFileException(ERROR_INFO, "Empty interface tag");
    }

    orca::FQInterfaceName fqIName;
    fqIName.platform = context.name().platform;
    fqIName.component = context.name().component;
    fqIName.iface = getProvidedName(context,ifaceTag);

    return fqIName;
}

orca::FQTopicName
getProvidedTopicWithTag( const Context& context, const std::string &ifaceTag, const std::string& subtopic )
{
    if ( ifaceTag.empty() ) {
        throw orcaice::ConfigFileException(ERROR_INFO, "Empty interface tag");
    }

    string ifaceName = getProvidedName( context, ifaceTag );

    return orcaice::toTopic( context.name(), ifaceName, subtopic );
}

std::string
getRequiredInterfaceAsString( const Context& context, const std::string &ifaceTag )
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
        initTracerError( context, errorString );
        throw orcaice::ConfigFileException(ERROR_INFO,errorString);
    }

    return proxy;
}

std::vector<std::string>
getProvidedTags( const Context& context, const std::string& pattern )
{
    std::string prefix = context.tag()+".Provides.";
    std::vector<std::string> tags = getFieldsForPrefix( context, prefix );

    if ( !pattern.empty() )
        filter_start( tags, pattern );

    return tags;
}

std::vector<std::string>
getRequiredTags( const Context& context, const std::string& pattern )
{
    std::string prefix = context.tag() + ".Requires.";
    std::vector<std::string> tags = getFieldsForPrefix( context, prefix );

    if ( !pattern.empty() )
        filter_start( tags, pattern );

    return tags;
}

orca::FQComponentName
resolveLocalPlatform( const orca::FQComponentName& fqname )
{
    if ( !fqname.platform.empty() && !fqname.component.empty() )
    {
        if ( fqname.platform=="local" )
        {
            orca::FQComponentName resolvedFqname;
            resolvedFqname.platform = hydroutil::getHostname();
            resolvedFqname.component = fqname.component;
            return resolvedFqname;
        }
    }
    // no changes were made
    return fqname;
}

orca::FQComponentName
resolveLocalPlatform( const Context& context, const orca::FQComponentName& fqname )
{
    if ( !fqname.platform.empty() && !fqname.component.empty() )
    {
        if ( fqname.platform=="local" )
        {
            orca::FQComponentName resolvedFqname;
            resolvedFqname.platform = hydroutil::getHostname();
            resolvedFqname.component = fqname.component;
            initTracerInfo( context, "resolved platform 'local' to hostname in component name '"+orcaice::toString( resolvedFqname )+"'" );
            return resolvedFqname;
        }
    }
    // no changes were made
    return fqname;
}

std::string
resolveLocalPlatform( const Context& context, const std::string& proxy )
{
    // for indirect proxies only: if platform is set to 'local', replace it by hostname
    orca::FQInterfaceName fqname = orcaice::toInterfaceName( proxy );
    if ( !fqname.iface.empty() && !fqname.platform.empty() && !fqname.component.empty() )
    {
        if ( fqname.platform=="local" )
        {
            fqname.platform = hydroutil::getHostname();
            string resolvedProxy = orcaice::toString( fqname );
            initTracerInfo( context, "resolved platform 'local' to hostname in proxy '"+resolvedProxy+"'" );
            return resolvedProxy;
        }
    }
    // no changes were made
    return proxy;
}

orca::ComponentData
getComponentData( const Context& context )
{
    orca::ComponentData compData;
    compData.name = context.name();

    Ice::PropertyDict providesProps =
        context.properties()->getPropertiesForPrefix(context.tag()+".Provides");
    orca::ProvidedInterface provided;
    for ( Ice::PropertyDict::iterator i=providesProps.begin(); i!=providesProps.end(); ++i ) {
        provided.name = i->second;
        provided.id = "unknown";
        compData.provides.push_back( provided );
    }

    // special cases: add standard interfaces (depending on startup flags)
    // first, add the Home interface itself
//     std::string homeIdentity = "orca." + context.name().platform + "." + context.name().component + "/Home";
//     provided.name = homeIdentity;
//     provided.id = "::orca::Home";
//     compData.provides.push_back( provided );
//
//     if ( interfaceFlag_& TracerInterface ) {
//         provided.name = "tracer";
//         provided.id = "::orca::Tracer";
//         compData.provides.push_back( provided );
//     }
//
//     if ( interfaceFlag_& StatusInterface ) {
//         provided.name = "status";
//         provided.id = "::orca::Status";
//         compData.provides.push_back( provided );
//     }

    // NOTE: this will not work if the config file uses long notation for req. interfaces
    Ice::PropertyDict requiresProps =
        context.properties()->getPropertiesForPrefix(context.tag()+".Requires");
    orca::RequiredInterface required;
    for ( Ice::PropertyDict::iterator i=requiresProps.begin(); i!=requiresProps.end(); ++i ) {
        required.name = orcaice::toInterfaceName( i->second );
        required.id = "unknown";
        compData.requires.push_back( required );
    }

    return compData;
}

} // namespace
