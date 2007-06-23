/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>

#include "privateutils.h"

#include "../orcaice.h"

using namespace std;

namespace orcaice
{
namespace detail
{

// Transfer a property from one property set to another
// returns:
//  0 if it was transferred successfully
//  1 if the property already existed in the target set and it was left untouched
// -1 if the property was not set in the source set, the target was left untouched
int
transferProperty( Ice::PropertiesPtr& fromProperties, Ice::PropertiesPtr& toProperties,
                        const string& fromKey, const string& toKey, bool force )
{
    // not forcing the transfer
    if ( !force ) {
        // check that the target does not have the property already set
        string toValue = toProperties->getProperty( toKey );
        bool existToValue = !toValue.empty();
        if ( existToValue ) {
            // there's something already in the target properties, don't touch it.
            return 1;
        }
        // debug
//         cout<<"DEBUG: nothing in the 'to' field: value='"<<toValue<<"'"<<endl;
    }

    string fromValue = fromProperties->getProperty( fromKey );
    bool existFromValue = !fromValue.empty();
    if ( !existFromValue ) {
        // the property is not set in the source set, leave the target one untouched
        return -1;
    }
    
    // transerring the value
    toProperties->setProperty( toKey, fromValue );
    return 0;
}

// Internal helper function.
// behaves like the function above. if key is missing, sets the toValue to defaultValue.
void
transferPropertyWithDefault( Ice::PropertiesPtr& fromProperties, Ice::PropertiesPtr& toProperties,
                      const string& fromKey, const string& toKey, const string& defaultValue, bool force )
{
    if ( !force ) {
        // if we asked not to force the transfer,
        // check that the target does not have the property already set
        string toValue = toProperties->getProperty( toKey );
        if ( !toValue.empty() ) {
            // there's something already in the target properties, don't touch it.
            return;
        }
    }

    string fromValue = fromProperties->getPropertyWithDefault( fromKey, defaultValue );
    toProperties->setProperty( toKey, fromValue );
}

// Internal helper function.
void
setPropertyDefault( Ice::PropertiesPtr& toProperties, const string& key, const string& defaultValue, bool force )
{
    if ( !force ) {
        // if we asked not to force the transfer,
        // check that the target does not have the property already set
        string toValue = toProperties->getProperty( key );
        if ( !toValue.empty() ) {
            // there's something already in the target properties, don't touch it.
            return;
        }
    }
    // target properties don't have this property set, set to default
    toProperties->setProperty( key, defaultValue );
}    

// this is just a place-holder, the code which is here right now is actually done in Application::Application()
void
parseOrcaCommandLineOptions( const Ice::StringSeq& args )
{
    for ( unsigned int i=0; i<args.size(); ++i ) {
        if ( !args[i].compare( 0,2, "-v" ) ||
                  !args[i].compare( 0,9, "--version" ) )
        {
            // print out Ice and libOrcaIce versions.
            orcaice::detail::printVersion();
            // nothing to clean up yet
            exit(0);
        }
    }
}

void
setFactoryProperties( Ice::PropertiesPtr& properties, const std::string& compTag )
{
    // Instantiate a separate property set
    Ice::PropertiesPtr tempProperties = Ice::createProperties();

    // modify Ice defaults
    // Ice default is 1 and it always prints out warning because we attach our own 
    // config parameters (e.g. Platform, Component, Config.XXX) to the tag of the
    // Object Adapter.
    tempProperties->setProperty( "Ice.Warn.UnknownProperties",    "0" );
//     tempProperties->setProperty( "Ice.Trace.Network",          "0" );
//     tempProperties->setProperty( "Ice.Trace.Protocol",         "0" );
//     tempProperties->setProperty( "Ice.Warn.Connections",       "0" );
//     tempProperties->setProperty( "Ice.PrintAdapterReady",      "1" );
//     tempProperties->setProperty( "Ice.Logger.Timestamp",       "1" );

    // the default assumes that there's an IceStorm server running on our host at
    // the specified port. This default can be over-written by specifying
    // --IceStorm.TopicManager.Proxy property.
    tempProperties->setProperty( "IceStorm.TopicManager.Proxy", "IceStorm/TopicManager:default -p 10000" );

    // adapter properties: these two are required for everything to work but
    // they are not present in the default config files. You can override these 
    // in the config files.
    orca::FQComponentName fqCName;
    // we've already made sure that component and platform are filled in
    fqCName.platform = properties->getProperty(compTag+".Platform");
    fqCName.component = properties->getProperty(compTag+".Component");
    tempProperties->setProperty( compTag+".AdapterId", orcaice::toString(fqCName) );
    tempProperties->setProperty( compTag+".Endpoints", "tcp -t 5000" );

    // orca properties
    tempProperties->setProperty( "Orca.PrintProperties",       "0" );
    tempProperties->setProperty( "Orca.PrintComponentStarted", "0" );
    tempProperties->setProperty( "Orca.RequireRegistry",       "1" );
    tempProperties->setProperty( "Orca.Warn.DefaultProperty",  "1" );
    tempProperties->setProperty( "Orca.Warn.FactoryProperty",  "0" );

    // all tracer tempProperties have default values
    tempProperties->setProperty( "Orca.Tracer.RequireIceStorm",    "0" );
    tempProperties->setProperty( "Orca.Tracer.Filename",           "orca_component_trace.txt" );
    // format
    tempProperties->setProperty( "Orca.Tracer.Timestamp",          "1" );
    // destinations
    tempProperties->setProperty( "Orca.Tracer.InfoToDisplay",      "1" );
    tempProperties->setProperty( "Orca.Tracer.InfoToNetwork",      "0" );
    tempProperties->setProperty( "Orca.Tracer.InfoToFile",         "0" );
    tempProperties->setProperty( "Orca.Tracer.WarningToDisplay",   "1" );
    tempProperties->setProperty( "Orca.Tracer.WarningToNetwork",   "0" );
    tempProperties->setProperty( "Orca.Tracer.WarningToFile",      "0" );
    tempProperties->setProperty( "Orca.Tracer.ErrorToDisplay",     "10" );
    tempProperties->setProperty( "Orca.Tracer.ErrorToNetwork",     "0" );
    tempProperties->setProperty( "Orca.Tracer.ErrorToFile",        "0" );
    tempProperties->setProperty( "Orca.Tracer.DebugToDisplay",     "0" );
    tempProperties->setProperty( "Orca.Tracer.DebugToNetwork",     "0" );
    tempProperties->setProperty( "Orca.Tracer.DebugToFile",        "0" );
    // filtering
    // these are not currently used.
//     tempProperties->setProperty( "Orca.Tracer.IgnoreRepeatedWarnings","0" );
//     tempProperties->setProperty( "Orca.Tracer.IgnoreRepeatedErrors",  "0" );

    // we are transfering prop's from lower to higher priority, this means that
    // if a property is already defined in the target property set, it is used and not the one from the source.
    bool forceTransfer = false;

    // this property is self-referncial, as a special case look it up with default.
    // it will also appear in the component property set.
    bool warnFactoryProp = (bool)properties->getPropertyAsIntWithDefault(
        "Orca.Warn.FactoryProperty", 0);

    std::map<string,string> props = tempProperties->getPropertiesForPrefix("");
    for ( std::map<string,string>::iterator it=props.begin(); it!=props.end(); ++it ) {
        bool ret = transferProperty( tempProperties, properties, it->first, it->first, forceTransfer );
        if ( warnFactoryProp && ret==0 ) {
            initTracerInfo( "Set property to factory default value: "+it->first+"="+it->second );
        }
    }
}

void
setGlobalProperties( Ice::PropertiesPtr& properties, const std::string& filename )
{    
    // Instantiate a separate property set
    Ice::PropertiesPtr tempProperties = Ice::createProperties();

    try 
    {
        tempProperties->load( filename );
    }
    catch ( Ice::SyscallException& e ) 
    {
        stringstream ss; ss<<"Couldn't load global configuration file '"<<filename<<"': "<<e;
        initTracerError( ss.str() );
        throw orcaice::Exception( ERROR_INFO, ss.str() );
    }

    // we are transfering prop's from lower to higher priority, this means that
    // if a property is already defined in the target property set, it is used and not the one from the source.
    bool forceTransfer = false;

    std::map<string,string> props = tempProperties->getPropertiesForPrefix("");
    for ( std::map<string,string>::iterator it=props.begin(); it!=props.end(); ++it ) {
        transferProperty( tempProperties, properties, it->first, it->first, forceTransfer );
    }
}

void
setComponentProperties( Ice::PropertiesPtr& properties, const std::string& filename )
{    
    // Instantiate a separate property set
    Ice::PropertiesPtr tempProperties = Ice::createProperties();

    try 
    {
        tempProperties->load( filename );
    }
    catch ( Ice::SyscallException& e ) 
    {
        stringstream ss;ss<<"Couldn't load component configuration file '"<<filename<<"': "<<e;
        throw orcaice::Exception( ERROR_INFO, ss.str() );
    }

    // debug
//     initTracerInfo("loaded component properties from file:");
//     orcaice::printComponentProperties( tempProperties, "Debug" );

    // we are transfering prop's from lower to higher priority, this means that
    // if a property is already defined in the target property set, it is used and not the one from the source.
    bool forceTransfer = false;

    std::map<string,string> props = tempProperties->getPropertiesForPrefix("");
    for ( std::map<string,string>::iterator it=props.begin(); it!=props.end(); ++it ) {
        transferProperty( tempProperties, properties, it->first, it->first, forceTransfer );
    }

    // extra funky stuff, this is needed for Services
    // still not forcing it
//     forceTransfer = false;
    // some properties may be duplicated under other keys
//     transferProperty( properties, properties, "Ice.Default.Locator", compTag + ".Locator", forceTransfer );
}

orca::FQComponentName
parseComponentProperties( const Ice::CommunicatorPtr& communicator, const std::string& compTag )
{
    Ice::PropertiesPtr properties = communicator->getProperties();

    // default was already set
    bool warnFactoryProp = (bool)properties->getPropertyAsInt("Orca.Warn.FactoryProperty");

    orca::FQComponentName fqCName;

    string adapter;
    // first, check for concise syntax: "platform/component"
    if ( !orcaice::getProperty( properties, compTag+".AdapterId", adapter ) ) {
        fqCName = orcaice::toComponentName( adapter );
    }
    else { 
    // otherwise, check extended syntax: platform and component separately
        fqCName.platform = properties->getProperty(compTag+".Platform");
        // lookup component name
        fqCName.component = properties->getProperty(compTag+".Component");
    }

    // check that we have platform name, if missing set it to 'local'
    if ( fqCName.platform.empty() ) {
        fqCName.platform = "local";
        if ( warnFactoryProp )
            initTracerInfo( "Set property to factory default value: "+compTag+".Platform="+fqCName.platform );
    }
    // check that we have component name, if missing set it to 'ComponentTag' converted to lower case
    if( fqCName.component.empty() )
    {
        fqCName.component = orcaice::toLowerCase( compTag );
        if ( warnFactoryProp )
            initTracerInfo( "Set property to factory default value: "+compTag+".Component="+fqCName.component );
    }

    // special case: replace 'local' platform with actual hostname
    if ( fqCName.platform == "local" ) {
        fqCName.platform = getHostname();
        // update properties
        properties->setProperty( compTag+".Platform", fqCName.platform );
        properties->setProperty( compTag+".AdapterId", orcaice::toString(fqCName) );
        if ( warnFactoryProp ) {
            initTracerInfo( "Replaced property (local<-hostname): "+compTag+".Platform="+fqCName.platform );
            initTracerInfo( "Replaced property (local<-hostname): "+compTag+".AdapterId="+orcaice::toString(fqCName) );
        }
    }

    return fqCName;
}

void
printComponentProperties( const Ice::PropertiesPtr& properties, const std::string& compTag )
{
    Ice::StringSeq propSeq = properties->getCommandLineOptions();

    std::ostringstream os;
    os << propSeq.size();

    initTracerInfo( compTag+": All component properties ("+os.str()+")" );
    initTracerInfo( "========================" );

    for ( unsigned int i=0; i<propSeq.size(); ++i ) {
        initTracerInfo( propSeq[i] );
    }
    initTracerInfo( "========================" );
}

void
printVersion()
{
    std::ostringstream os;
    os << "Ice version: "<<ICE_STRING_VERSION<<" libOrcaIce version: "<<orcaice::orcaVersion();
    initTracerInfo( os.str() );
}

} // namespace
} // namespace
