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

#include "privateutils.h"

#include "../orcaice.h"

using namespace std;

namespace orcaice
{

// Internal helper function.
// Transfer a property from one property set to another
// returns 0 if it was transferred successfully
int
transferProperty( Ice::PropertiesPtr &fromProperties, Ice::PropertiesPtr &toProperties,
                        const string &fromKey, const string &toKey, bool force )
{
    if ( !force ) {
        // if we asked not to force the transfer,
        // check that the target does not have the property already set
        string toValue = toProperties->getProperty( toKey );
        if ( !toValue.empty() ) {
            // there's something already in the target properties, don't touch it.
            return 0;
        }
    }

    string fromValue = fromProperties->getProperty( fromKey );
    if ( fromValue.empty() )
    {
        return -1;
    }
    toProperties->setProperty( toKey, fromValue );
    return 0;
}

// Internal helper function.
// behaves like the function above. if key is missing, sets the toValue to defaultValue.
void
transferPropertyWithDefault( Ice::PropertiesPtr &fromProperties, Ice::PropertiesPtr &toProperties,
                      const string &fromKey, const string &toKey, const string &defaultValue, bool force )
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
setPropertyDefault( Ice::PropertiesPtr &toProperties, const string &key, const string &defaultValue, bool force )
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
parseOrcaCommandLineOptions( const Ice::StringSeq & args )
{
    for ( unsigned int i=0; i<args.size(); ++i ) {
        if ( !args[i].compare( 0,2, "-v" ) ||
                  !args[i].compare( 0,9, "--version" ) )
        {
            // print out Ice and libOrcaIce versions.
            orcaice::printVersion();
            // nothing to clean up yet
            exit(0);
        }
    }
}

void
setFactoryProperties( Ice::PropertiesPtr &properties, const std::string &compTag )
{
    // Instantiate a separate property set
    Ice::PropertiesPtr tempProperties = Ice::createProperties();

    // orca properties
    tempProperties->setProperty( "Orca.PrintProperties",       "0" );
    tempProperties->setProperty( "Orca.PrintComponentStarted", "0" );
    tempProperties->setProperty( "Orca.RequireRegistry",       "1" );

    // @todo This is a big assumption.
    // the default assumes that there's an IceStorm server running on our host at
    // the specified port. This does not preclude us from actually running several
    // servers, but the events published to the other servers will not be available
    // to subscribers using this default configuration.
    tempProperties->setProperty( "IceStorm.TopicManager.Proxy", "IceStorm/TopicManager:default -p 10000" );

    // modify Ice defaults
//     tempProperties->setProperty( "Ice.Trace.Network",          "0" );
//     tempProperties->setProperty( "Ice.Trace.Protocol",         "0" );
//     tempProperties->setProperty( "Ice.Warn.Connections",       "0" );
//     tempProperties->setProperty( "Ice.PrintAdapterReady",      "1" );
//     tempProperties->setProperty( "Ice.Logger.Timestamp",       "1" );


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
    tempProperties->setProperty( "Orca.Tracer.IgnoreRepeatedWarnings","0" );
    tempProperties->setProperty( "Orca.Tracer.IgnoreRepeatedErrors",  "0" );

    // we are transfering prop's from lower to higher priority, this means that
    // if a property is already defined in the target property set, it is used and not the one from the source.
    bool forceTransfer = false;

    std::map<string,string> props = tempProperties->getPropertiesForPrefix("");
    for ( std::map<string,string>::iterator it=props.begin(); it!=props.end(); ++it ) {
        transferProperty( tempProperties, properties, it->first, it->first, forceTransfer );
    }
}

void
setGlobalProperties( Ice::PropertiesPtr & properties, const std::string & filename )
{    
    // Instantiate a separate property set
    Ice::PropertiesPtr tempProperties = Ice::createProperties();

    try 
    {
        tempProperties->load( filename );
    }
    catch ( Ice::SyscallException &e ) 
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
setComponentProperties( Ice::PropertiesPtr & properties, const std::string & filename )
{    
    // Instantiate a separate property set
    Ice::PropertiesPtr tempProperties = Ice::createProperties();

    try 
    {
        tempProperties->load( filename );
    }
    catch ( Ice::SyscallException &e ) 
    {
        stringstream ss;ss<<"Couldn't load component configuration file '"<<filename<<"': "<<e;
        throw orcaice::Exception( ERROR_INFO, ss.str() );
    }

    // debug
//     initTracerPrint("loaded component properties from file:");
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

// This function is called before the tracer is created, so we can't use it.
orca::FQComponentName
parseComponentProperties( const Ice::CommunicatorPtr & communicator, const std::string & compTag )
{
    Ice::PropertiesPtr properties = communicator->getProperties();

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
        string warnString = compTag + ": " + warnMissingPropertyWithDefault( compTag+".Platform", "local" );
        initTracerWarning( warnString );
        fqCName.platform = "local";
    }
    // check that we have component name, if missing set it to 'ComponentTag' converted to lower case
    if( fqCName.component.empty() )
    {
//         if ( we are picky ) {
//             initTracerError( errorString );
//             throw orcaice::Exception( ERROR_INFO, errorString );
//         }
        string warnString = compTag + ": " + warnMissingPropertyWithDefault( compTag+".Component", "<component tag>" );
        initTracerWarning( warnString );
        fqCName.component = orcaice::toLowerCase( compTag );
    }

    // special case: replace 'local' platform with actual hostname
    if ( fqCName.platform == "local" ) {
        fqCName.platform = getHostname();
        // update properties
        properties->setProperty( compTag+".Platform", fqCName.platform );
        properties->setProperty( compTag+".AdapterId", orcaice::toString(fqCName) );
        initTracerWarning( compTag+": Set platform name to hostname="+fqCName.platform );
    }

    // combine platform and component names into adapter ID
    properties->setProperty( compTag+".AdapterId", orcaice::toString(fqCName) );

    initTracerPrint( compTag+": Will register component (adapter) as '"+orcaice::toString(fqCName)+"'" );

    return fqCName;
}

void
printComponentProperties( const Ice::PropertiesPtr &properties, const std::string &compTag )
{
    Ice::StringSeq propSeq = properties->getCommandLineOptions();

    std::ostringstream os;
    os << propSeq.size();

    initTracerPrint( compTag+": All component properties ("+os.str()+")" );
    initTracerPrint( "========================" );

    for ( unsigned int i=0; i<propSeq.size(); ++i ) {
        initTracerPrint( propSeq[i] );
    }
    initTracerPrint( "========================" );
}

void
printVersion()
{
    std::ostringstream os;
    os << "Ice version: "<<ICE_STRING_VERSION<<" libOrcaIce version: "<<orcaice::orcaVersion();
    initTracerPrint( os.str() );
}

} // namespace
