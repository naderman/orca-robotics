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

#include "privateutils.h"
#include "propfileutils.h"

#include <orcaice/component.h>
#include <orcaice/orcaice.h>
#include <orcaice/icegridutils.h>
#include <orca/properties.h>
#include <IceGrid/Registry.h>  // used to register Home interface as a well-known object

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
transferProperty( const Ice::PropertiesPtr& fromProps, 
                  const Ice::PropertiesPtr& toProps,
                  const string&             fromKey,
                  const string&             toKey,
                  bool                      force )
{
    string fromValue = fromProps->getProperty( fromKey );
    bool existFromValue = !fromValue.empty();
    if ( !existFromValue ) {
        // the property is not set in the source set, leave the target one untouched
        return -1;
    }
    return transferProperty( toProps,
                             fromKey,
                             fromValue,
                             toKey,
                             force );
}

// Transfer a property from one property set to another
// returns:
//  0 if it was transferred successfully
//  1 if the property already existed in the target set and it was left untouched
int
transferProperty( const Ice::PropertiesPtr& toProps,
                  const string&             fromKey,
                  const string&             fromValue,
                  const string&             toKey,
                  bool                      force )
{
    // not forcing the transfer
    if ( !force ) {
        // check that the target does not have the property already set
        string toValue = toProps->getProperty( toKey );
        bool existToValue = !toValue.empty();
        if ( existToValue ) {
            // there's something already in the target properties, don't touch it.
            return 1;
        }
        // debug
//         cout<<"DEBUG: nothing in the 'to' field: value='"<<toValue<<"'"<<endl;
    }

    // transerring the value
    toProps->setProperty( toKey, fromValue );
    return 0;
}

// Internal helper function.
// behaves like transferProperty. if key is missing, sets the toValue to defaultValue.
void
transferPropertyWithDefault( const Ice::PropertiesPtr& fromProps, 
                             const Ice::PropertiesPtr& toProps,
                             const string&             fromKey,
                             const string&             toKey,
                             const string&             defaultValue,
                             bool                      force )
{
    if ( !force ) {
        // if we asked not to force the transfer,
        // check that the target does not have the property already set
        string toValue = toProps->getProperty( toKey );
        if ( !toValue.empty() ) {
            // there's something already in the target properties, don't touch it.
            return;
        }
    }

    string fromValue = fromProps->getPropertyWithDefault( fromKey, defaultValue );
    toProps->setProperty( toKey, fromValue );
}

// Internal helper function.
void
setPropertyDefault( Ice::PropertiesPtr& toProps, const string& key, const string& defaultValue, bool force )
{
    if ( !force ) {
        // if we asked not to force the transfer,
        // check that the target does not have the property already set
        string toValue = toProps->getProperty( key );
        if ( !toValue.empty() ) {
            // there's something already in the target properties, don't touch it.
            return;
        }
    }
    // target properties don't have this property set, set to default
    toProps->setProperty( key, defaultValue );
}    

void
setFactoryProperties( Ice::PropertiesPtr& props, const std::string& compTag )
{
    // Instantiate a separate property set
    Ice::PropertiesPtr tempProps = Ice::createProperties();

    // modify Ice defaults
    // Ice default is 1 and it always prints out warning because we attach our own 
    // config parameters (e.g. Platform, Component, Config.XXX) to the tag of the
    // Object Adapter.
    tempProps->setProperty( "Ice.Warn.UnknownProperties",    "0" );
//     tempProps->setProperty( "Ice.Trace.Network",          "0" );
//     tempProps->setProperty( "Ice.Trace.Protocol",         "0" );
//     tempProps->setProperty( "Ice.Warn.Connections",       "0" );
//     tempProps->setProperty( "Ice.PrintAdapterReady",      "1" );
//     tempProps->setProperty( "Ice.Logger.Timestamp",       "1" );

    // the default assumes that there's an IceStorm server running on our host at
    // the specified port. This default can be over-written by specifying
    // --IceStorm.TopicManager.Proxy property.
    tempProps->setProperty( "IceStorm.TopicManager.Proxy", "IceStorm/TopicManager:default -p 10000" );

    // adapter properties: AdapterId and Endpoints are required for everything to work but
    // they are not present in the default config files. You can override these 
    // in the config files.
    // Note: we no longer set AdapterId here. See postProcessComponentProperties().
    tempProps->setProperty( compTag+".Endpoints", "tcp -t 5000" );

    // orca properties
    tempProps->setProperty( "Orca.Warn.DefaultProperty",  "1" );
    tempProps->setProperty( "Orca.Warn.FactoryProperty",  "0" );
    
    // Application properties
    // This is an advanced property. Default value should be good for most cases.
    // The alternative is to request ShutdownOnInterrupt.
    tempProps->setProperty( "Orca.Application.CallbackOnInterrupt",  "1" );

    // Component properties
    tempProps->setProperty( "Orca.Component.RequireRegistry",  "1" );
    tempProps->setProperty( "Orca.Component.EnableProperties", "1" );
    tempProps->setProperty( "Orca.Component.EnableProcess",    "0" );
    // these 3 are special: the defaults are set by the programmer in the source code
//     tempProps->setProperty( "Orca.Component.EnableTracer",  "1" );
//     tempProps->setProperty( "Orca.Component.EnableStatus",  "1" );
//     tempProps->setProperty( "Orca.Component.EnableHome",    "1" );
    tempProps->setProperty( "Orca.Component.PrintProperties",  "0" );
    tempProps->setProperty( "Orca.Component.PrintContext",     "0" );
    tempProps->setProperty( "Orca.Component.PrintStarted",     "0" );

    // Status properties
    tempProps->setProperty( "Orca.Status.RequireIceStorm",    "0" );

    // all tracer tempProps have default values
    tempProps->setProperty( "Orca.Tracer.RequireIceStorm",    "0" );
    tempProps->setProperty( "Orca.Tracer.Filename",           "orca_component_trace.txt" );
    // format
    tempProps->setProperty( "Orca.Tracer.Timestamp",          "1" );
    // destinations
    tempProps->setProperty( "Orca.Tracer.InfoToDisplay",      "1" );
    tempProps->setProperty( "Orca.Tracer.InfoToNetwork",      "0" );
    tempProps->setProperty( "Orca.Tracer.InfoToFile",         "0" );
    tempProps->setProperty( "Orca.Tracer.WarningToDisplay",   "1" );
    tempProps->setProperty( "Orca.Tracer.WarningToNetwork",   "0" );
    tempProps->setProperty( "Orca.Tracer.WarningToFile",      "0" );
    tempProps->setProperty( "Orca.Tracer.ErrorToDisplay",     "10" );
    tempProps->setProperty( "Orca.Tracer.ErrorToNetwork",     "0" );
    tempProps->setProperty( "Orca.Tracer.ErrorToFile",        "0" );
    tempProps->setProperty( "Orca.Tracer.DebugToDisplay",     "0" );
    tempProps->setProperty( "Orca.Tracer.DebugToNetwork",     "0" );
    tempProps->setProperty( "Orca.Tracer.DebugToFile",        "0" );
    // filtering
    // these are not currently used.
//     tempProps->setProperty( "Orca.Tracer.IgnoreRepeatedWarnings","0" );
//     tempProps->setProperty( "Orca.Tracer.IgnoreRepeatedErrors",  "0" );

    // we are transfering prop's from lower to higher priority, this means that
    // if a property is already defined in the target property set, it is used and not the one from the source.
    bool forceTransfer = false;

    // this property is self-referncial, as a special case look it up with default.
    // it will also appear in the component property set.
    bool warnFactoryProp = (bool)props->getPropertyAsIntWithDefault(
        "Orca.Warn.FactoryProperty", 0);

    std::map<string,string> propMap = tempProps->getPropertiesForPrefix("");
    for ( std::map<string,string>::iterator it=propMap.begin(); it!=propMap.end(); ++it ) {
        bool ret = transferProperty( tempProps, props, it->first, it->first, forceTransfer );
        if ( warnFactoryProp && ret==0 ) {
            initTracerInfo( "Set property to factory default value: "+it->first+"="+it->second );
        }
    }
}

void
setGlobalProperties( Ice::PropertiesPtr& props, const std::string& filename )
{    
    // Instantiate a separate property set
    Ice::PropertiesPtr tempProps = Ice::createProperties();

    try 
    {
        tempProps->load( filename );
    }
    catch ( Ice::SyscallException& e ) 
    {
        stringstream ss; ss<<"Couldn't load global configuration file '"<<filename<<"': "<<e;
        initTracerError( ss.str() );
        throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
    }

    // we are transfering prop's from lower to higher priority, this means that
    // if a property is already defined in the target property set, it is used and not the one from the source.
    bool forceTransfer = false;

    std::map<string,string> propMap = tempProps->getPropertiesForPrefix("");
    for ( std::map<string,string>::iterator it=propMap.begin(); it!=propMap.end(); ++it ) {
        transferProperty( tempProps, props, it->first, it->first, forceTransfer );
    }
}

void
setComponentPropertiesFromFile( Ice::PropertiesPtr& props, const std::string& filename )
{    
    // Instantiate a separate property set
    Ice::PropertiesPtr tempProps = Ice::createProperties();

    try 
    {
        tempProps->load( filename );
    }
    catch ( Ice::SyscallException& e ) 
    {
        stringstream ss;ss<<"Couldn't load component configuration file '"<<filename<<"': "<<e;
        throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
    }

    // debug
//     initTracerInfo("loaded component properties from file:");
//     orcaice::printComponentProperties( tempProps, "Debug" );

    // we are transfering prop's from lower to higher priority, this means that
    // if a property is already defined in the target property set, it is used and not the one from the source.
    bool forceTransfer = false;

    std::map<string,string> propMap = tempProps->getPropertiesForPrefix("");
    for ( std::map<string,string>::iterator it=propMap.begin(); it!=propMap.end(); ++it ) {
        transferProperty( tempProps, props, it->first, it->first, forceTransfer );
    }

    // extra funky stuff, this is needed for Services
    // still not forcing it
//     forceTransfer = false;
    // some properties may be duplicated under other keys
//     transferProperty( props, props, "Ice.Default.Locator", compTag + ".Locator", forceTransfer );
}

// alexm: copied over from orcaobj/stringutils.h in order to cut dependency
std::string toString( const orca::PropertiesData &obj )
{
    std::ostringstream s;
    s << " PropertiesData ["<<obj.properties.size() << " elements]:"<<endl;
    for ( map<string,string>::const_iterator it = obj.properties.begin();
          it != obj.properties.end();
          it++ )
    {
        s << "  " << it->first << "=" << it->second << endl;
    }
    return s.str();
}

void
setComponentPropertiesFromServer( const Context& context )
{
    // If _anything_ goes wrong, throws Exception
    try {
        // Connect to the remote properties server
        std::string propertyServerProxyString = orcaice::getPropertyWithDefault( context.properties(), 
                                                                                 "Orca.PropertyServerProxyString",
                                                                                 "" );
        if ( propertyServerProxyString.empty() )
            return;

        // Get the properties from the remote properties server
        orca::PropertiesPrx propertyPrx;
        orcaice::connectToInterfaceWithString( context, propertyPrx, propertyServerProxyString );
        orca::PropertiesData propData = propertyPrx->getData();
        const std::map<std::string,std::string> &netProps = propData.properties;

        stringstream ssProps;
        ssProps << "setComponentPropertiesFromServer(): got network properties: " << toString(propData);
        context.tracer().debug( ssProps.str(), 3 );

        // Copy them into our properties, without over-writing anything that's already set
        bool forceTransfer = false;

        for ( std::map<string,string>::const_iterator it=netProps.begin(); it!=netProps.end(); ++it ) 
        {
            const string &fromKey   = it->first;
            const string &fromValue = it->second;
            const string &toKey     = it->first;
            int ret = transferProperty( context.properties(), fromKey, fromValue, toKey, forceTransfer );
            stringstream ss;
            if ( ret == 0 )
            {
                ss << "setComponentPropertiesFromServer(): transferred property '"
                   <<it->first<<"' -> '"<<it->second<<"'";
            }
            else
            {
                ss << "setComponentPropertiesFromServer(): retreived network property '"
                   <<it->first<<"' but did not over-write existing value";
            }
            context.tracer().debug( ss.str() );
        }
    }
    catch ( const std::exception &e ) {
        std::stringstream ss; ss << "(while getting network config properties) caught exception: " << e.what();
        throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
    }
    catch ( const std::string &e ) {
        std::stringstream ss; ss << "(while getting network config properties) caught std::string: " << e;
        throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
    }
    catch ( const char* &e ) {
        std::stringstream ss; ss << "(while getting network config properties) caught char*: " << e;
        throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
    }
    catch ( ... ) {
        std::stringstream ss; ss << "(while getting network config properties) caught unknown exception.";
        throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
    }
}

void
postProcessComponentProperties( const Ice::PropertiesPtr& props, const std::string& compTag )
{
    // all defaults have already been set!

    bool warnFactoryProp = (bool)props->getPropertyAsInt("Orca.Warn.FactoryProperty");

    orca::FQComponentName fqCName;
    string adapter;

    // first, check for concise syntax: "platform/component"
    // NOTE: this feature is undocumented.
    // alexm: I think this we don't document it to simplify explanation of config files,
    // but we need this for running components through IceGrid.
    if ( !orcaice::getProperty( props, compTag+".AdapterId", adapter ) ) 
    {
cout<<"DEBUG: adapterId is specified"<<endl;
        // When AdapterId is specified, it overwrites platform and component properties.
        fqCName = orcaice::toComponentName( adapter );
        props->setProperty( compTag+".Platform", fqCName.platform );
        props->setProperty( compTag+".Component", fqCName.component );
    }
    else 
    { 
cout<<"DEBUG: adapterId is NOT specified"<<endl;
        // When AdapterId is NOT specified, we read individual properties and set the AdapterId
        // (at this point, these properties may be empty)
        fqCName.platform = props->getProperty( compTag+".Platform" );
        fqCName.component = props->getProperty( compTag+".Component" );
        props->setProperty( compTag+".AdapterId", orcaice::toString(fqCName) );
    }

    // check that we have platform name, if it's missing set it to 'local'
    // this may easily happen if the component is executed without a config file.
    if ( fqCName.platform.empty() ) 
    {
        fqCName.platform = "local";
        if ( warnFactoryProp )
            initTracerInfo( "Set property to factory default value: "+compTag+".Platform="+fqCName.platform );
        props->setProperty( compTag+".Platform", fqCName.platform );
        props->setProperty( compTag+".AdapterId", orcaice::toString(fqCName) );
    }

    // check that we have component name, if missing set it to 'ComponentTag' converted to lower case.
    // this may easily happen if the component is executed without a config file.
    if( fqCName.component.empty() )
    {
        fqCName.component = hydroutil::toLowerCase( compTag );
        if ( warnFactoryProp )
            initTracerInfo( "Set property to factory default value: "+compTag+".Component="+fqCName.component );
        props->setProperty( compTag+".Component", fqCName.component );
        props->setProperty( compTag+".AdapterId", orcaice::toString(fqCName) );
    }

    // special case: replace 'local' platform with actual hostname
    if ( fqCName.platform == "local" ) 
    {
        fqCName.platform = hydroutil::getHostname();
        // update properties
        props->setProperty( compTag+".Platform", fqCName.platform );
        props->setProperty( compTag+".AdapterId", orcaice::toString(fqCName) );
        if ( warnFactoryProp ) {
            initTracerInfo( "Replaced 'local' with hostname in platform name : "+compTag+".Platform="+fqCName.platform );
            initTracerInfo( "Replaced 'local' with hostname in adapter ID : "+compTag+".AdapterId="+orcaice::toString(fqCName) );
        }
    }
    else {
cout<<"DEBUG: NOT replacing local platform"<<endl;
    }

    // 
    // Admin interface
    //
    // the component will decide later whether to enable the interface or not.
    props->setProperty( "Ice.Admin.Endpoints", "tcp -t 5000" );
    string adminInstanceName = fqCName.platform + "." + fqCName.component;
    props->setProperty( "Ice.Admin.InstanceName", adminInstanceName );
    // I don't think we need the delay actually. All settings are set before the communicator is created.
//     props->setProperty( "Ice.Admin.DelayCreation", "1" );

    string facetsToLoad;
    // We need to deal with this clunky filtering because that's the only way to control which standard
    // Admin facets will be loaded.
    // Ice standard facets: use configs to decide which ones we want Ice to load
    if ( props->getPropertyAsInt( "Orca.Component.EnableProperties" ) )
        facetsToLoad += "Properties";
    if ( props->getPropertyAsInt( "Orca.Component.EnableProcess" ) )
        facetsToLoad += " Process";
    // Orca custom facets: we add all of them to the list of allowed facets and compoent will later start the ones it wants
    // (if we don't white-list them here, Ice will not make them visible)
    facetsToLoad += " " + orcaice::toAdminFacet( fqCName, "::orca::Home" );
    facetsToLoad += " " + orcaice::toAdminFacet( fqCName, "::orca::Status" );
    facetsToLoad += " " + orcaice::toAdminFacet( fqCName, "::orca::Tracer" );
    props->setProperty( "Ice.Admin.Facets", facetsToLoad );
}

void
printComponentContext( const orcaice::Context& context )
{
    initTracerInfo( context.tag()+": Contents of the component's context" );
    cout<< "========================"<<endl;
    cout<<context.toString()<<endl;
    cout<< "========================"<<endl;
}

void
printComponentProperties( const Ice::PropertiesPtr& props, const std::string& compTag )
{
    Ice::StringSeq propSeq = props->getCommandLineOptions();

    std::ostringstream os;
    os << propSeq.size();

    initTracerInfo( compTag+": All component properties ("+os.str()+")" );
    cout<< "========================"<<endl;

    for ( unsigned int i=0; i<propSeq.size(); ++i ) {
        cout<<propSeq[i]<<endl;
    }
    cout<< "========================"<<endl;
}

void
printAllVersions( const Component& component )
{
    std::stringstream ss;
    ss << "Versions: Ice="<<ICE_STRING_VERSION<<"  Orca="<<hydroutil::orcaVersion();
    // if this is NOT an Orca component, print its version as well
    if ( !component.version().empty() ) {
        ss << "  Project="<<component.version();
    }
    initTracerInfo( ss.str() );
}

void addPropertiesFromApplicationConfigFile( Ice::PropertiesPtr   &props,
                                             const Ice::StringSeq &commandLineArgs,
                                             const std::string    &componentTag )
{
    std::string compFilename;
    try
    {
        compFilename = orcaice::detail::getApplicationConfigFilename( commandLineArgs );
        if ( compFilename.empty() ) {
            initTracerInfo( componentTag+": "+warnMissingProperty("component properties file","Orca.Config") );
        }
        else {
            orcaice::detail::setComponentPropertiesFromFile( props, compFilename );
            initTracerInfo( componentTag+": Loaded component properties from '"+compFilename+"'" );
        }
    }
    catch ( const gbxutilacfr::Exception &e )
    {
        initTracerWarning( componentTag+": Failed to open component config file '"+compFilename+"':"+e.what() );
    }    
}

void addPropertiesFromServiceConfigFile( Ice::PropertiesPtr   &props,
                                         const Ice::StringSeq &commandLineArgs,
                                         const std::string    &componentTag )
{
    if ( commandLineArgs.empty() ) {
        initTracerInfo( componentTag+": Empty command line options, cannot determine component config file." );
        return;
    }

    std::string servFilename;
    try
    {
        servFilename = orcaice::detail::getServiceConfigFilename( commandLineArgs );
        if ( servFilename.empty() ) {
            initTracerInfo( componentTag+": Component config file is not specified." );
        }
        else {
            orcaice::detail::setComponentPropertiesFromFile( props, servFilename );
            initTracerInfo( componentTag+": Loaded component properties from '"+servFilename+"'" );
        }
    }
    catch ( const gbxutilacfr::Exception &e )
    {
        initTracerWarning( componentTag+": Failed to open component config file : '"+servFilename+"'"+e.what() );
    }
}

void addPropertiesFromGlobalConfigFile( Ice::PropertiesPtr   &props,
                                        const std::string    &componentTag )
{
    std::string globFilename;
    try
    {
        globFilename = orcaice::detail::getGlobalConfigFilename( props );
        orcaice::detail::setGlobalProperties( props, globFilename );
        initTracerInfo( componentTag+": Loaded global properties from '"+globFilename+"'" );
    }
    catch ( const gbxutilacfr::Exception &e )
    {
        initTracerWarning( componentTag+": Failed to open global config file '"+globFilename+"': "+e.what() );
    }
}

void registerHomeInterface( const Context& context )
{
//     std::string homeIdentityString = toHomeIdentity( context.name() );
//     Ice::Identity homeIdentity = context.communicator()->stringToIdentity(homeIdentityString);

    // previous method: adding Home to the component adapter
//     Ice::ObjectPrx homePrx = context.adapter()->createProxy( homeIdentity );

    // new method: adding Home as a facet to the Admin interface.
    Ice::ObjectPrx adminPrx = context.communicator()->getAdmin();
    assert( adminPrx && "Null admin proxy when registering home" );
//     cout<<"DEBUG: admin proxy: "<<adminPrx->ice_toString()<<endl;

    // change generic proxy to the Home facet.
    // (strictly speaking, this is not necessary: we can always change the facet later. but it makes a bit clearer
    // if the facet is listed in the Registry).
    Ice::ObjectPrx homePrx = adminPrx->ice_facet( orcaice::toAdminFacet( context.name(), "::orca::Home" ) );

    // apparently cannot change just the registered name. it changes the proxy, which is wrong.
//     homePrx = homePrx->ice_identity( homeIdentity );
//     cout<<"DEBUG: home proxy: "<<homePrx->ice_toString()<<endl;

//     context.tracer().info( string("Registering Home with identity ")+homeIdentityString );

    std::string instanceName = context.properties()->getPropertyWithDefault( "IceGrid.InstanceName", "IceGrid" );
    Ice::ObjectPrx base = context.communicator()->stringToProxy( instanceName+"/Registry" );
    try {
        // Open an admin session with the registry
        IceGrid::RegistryPrx registry = IceGrid::RegistryPrx::checkedCast(base);
        // This assumes no access control
        std::string username = "componentthread-no-access-control";
        std::string password = "componentthread-no-access-control";
        IceGrid::AdminSessionPrx adminSession = registry->createAdminSession( username, password );

        //
        // use the adminSession to add our Home interface
        //
        IceGrid::AdminPrx admin = adminSession->getAdmin();
        try {
            admin->addObjectWithType( homePrx, "::orca::Home" );
        }
        catch ( const IceGrid::ObjectExistsException& ) {
            admin->updateObject( homePrx );
        }
        context.tracer().info( string("Registered Home as: ")+homePrx->ice_toString() );

        // we don't need the session anymore
        // (we can just leave it there and it would be destroyed eventually without being kept alive, but it's
        // more transparent if we destroy it ourselves)
        adminSession->destroy();
    }
    catch ( Ice::CommunicatorDestroyedException& ) 
    {
        context.tracer().info( "(while registering Home interface) communicator destroyed, ignoring." );
    }
}

} // namespace
} // namespace
