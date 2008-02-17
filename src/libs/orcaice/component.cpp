/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#include <IceStorm/IceStorm.h> // used in initTracer()
#include <string>
#include <orca/orca.h>
#include <orca/properties.h>
#include <orcaice/orcaice.h>
#include <hydroiceutil/localstatus.h>
#include <hydroiceutil/localtracer.h>
#include <hydroiceutil/localhistory.h>
#include "localhome.h"
#include "homeI.h"
#include "statusI.h"
#include "tracerI.h"
#include "privateutils.h"
#include "component.h"
#include "detail/componentthread.h"
#include "detail/privateutils.h"
#include <orcaobj/stringutils.h>

// debug only
#include <iostream>
using namespace std;

namespace orcaice {

namespace {

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

}

//////////////////////////////

Component::Component( const std::string& tag, ComponentInterfaceFlag flag )
    : interfaceFlag_(flag),
      componentThread_(0)
{
    context_.tag_ = tag;
}

Component::~Component()
{
}

void
Component::init( const orca::FQComponentName& name,
                 const bool isApp,
                 const Ice::ObjectAdapterPtr& adapter )
{
    // set context with component info
    // this is the only storage of this info
    context_.init( name, isApp, adapter, this );

    //
    // Initialize component services
    // Tracer is created first so that it can be used as soon as possible.
    //
    context_.tracer_ = initTracer();
    context_.status_ = initStatus();
    getNetworkProperties();
    context_.home_   = initHome();
    context_.history_= initHistory();
    componentThread_ = new ComponentThread( homePrx_, *(context_.status_), interfaceFlag_, context_ );
    try {
        componentThread_->start();
    }
    catch ( const Ice::Exception &e )
    {
        std::stringstream ss; ss << "orcaice::Component::start(): caught exception: " << e;
        context_.tracer().error( ss.str() );
        throw;
    }
    catch ( const std::exception &e )
    {
        std::stringstream ss; ss << "orcaice::Component::start(): caught exception: " << e.what();
        context_.tracer().error( ss.str() );
        throw;
    }
    catch ( const std::string &e )
    {
        std::stringstream ss; ss << "orcaice::Component::start(): caught std::string: " << e;
        context_.tracer().error( ss.str() );
        throw;
    }
    catch ( const char* &e )
    {
        std::stringstream ss; ss << "orcaice::Component::start(): caught char*: " << e;
        context_.tracer().error( ss.str() );
        throw;
    }
    catch ( ... )
    {
        std::stringstream ss; ss << "orcaice::Component::start(): caught unknown exception.";
        context_.tracer().error( ss.str() );
        throw;
    }
};

void
Component::finalise()
{
    if ( componentThread_ )
    {
        context_.tracer().debug( "orcaice::Component: stopping ComponentThread....", 2 );
        hydroiceutil::stopAndJoin( componentThread_ );
        context_.tracer().debug( "orcaice::Component: ComponentThread stopped.", 2 );
    }
}

hydroutil::Tracer*
Component::initTracer()
{
    // We use programmatic configration as default. Config file settings will always overwrite.
    bool enableInterface = interfaceFlag_ & TracerInterface;
    enableInterface = context_.properties()->getPropertyAsIntWithDefault( "Orca.Component.EnableTracer", enableInterface );
    // in case the settings have changed, update the flag (it may be used by someone in the future)
    if ( enableInterface )
        interfaceFlag_ = ComponentInterfaceFlag( interfaceFlag_ | TracerInterface );
    else
        interfaceFlag_ = ComponentInterfaceFlag( interfaceFlag_ & ~TracerInterface );

    if ( !enableInterface ) 
    {
        orcaice::initTracerInfo( context_.tag()+": Initialized local trace handler.");
        return new hydroiceutil::LocalTracer( 
                hydroutil::Properties( context_.properties()->getPropertiesForPrefix("Orca.Tracer.")),
                orcaice::toString(context_.name()) );
    }
        
    // this is a bit tricky. we need
    // 1. a smart pointer which derives from Object (or ObjectPtr itself) to add to adapter
    // 2. a smart pointer which derives from Tracer to save in context
    // Ideally we'd have something like StatusTracerPtr which does derive from both.
    // but the smart pointer stuff is then included twice and reference counters get confused.
    // So first we use the pointer to hydroutil::StatusTracerI, then change to Ice::ObjectPtr and Tracer*.
    orcaice::detail::TracerI* pobj = new orcaice::detail::TracerI( context_ );
    Ice::ObjectPtr obj = pobj;
    //TracerPtr trac = pobj;
    // have to revert to using plain pointers. Otherwise, we get segfault on shutdown when
    // trac tries to delete the object which already doesn't exist. Something wrong with ref counters.
    hydroutil::Tracer* trac = (hydroutil::Tracer*)pobj;
    
    //
    // add this object to the adapter and name it 'tracer'
    // 
    context_.adapter()->add( obj, context_.communicator()->stringToIdentity("tracer") );

    // a bit of a hack: keep this smart pointer so it's not destroyed with the adapter
    tracerObj_ = obj;
    
    orcaice::initTracerInfo( context_.tag()+": Initialized trace handler.");
    return trac;
}

hydroutil::Status*
Component::initStatus()
{
    // We use programmatic configration as default. Config file settings will always overwrite.
    bool enableInterface = interfaceFlag_ & StatusInterface;
    enableInterface = context_.properties()->getPropertyAsIntWithDefault( "Orca.Component.EnableStatus", enableInterface );
    // in case the settings have changed, update the flag (it may be used by someone in the future)
    if ( enableInterface )
        interfaceFlag_ = ComponentInterfaceFlag( interfaceFlag_ | StatusInterface );
    else
        interfaceFlag_ = ComponentInterfaceFlag( interfaceFlag_ & ~StatusInterface );

    if ( !enableInterface ) 
    {
        orcaice::initTracerInfo( context_.tag()+": Initialized local status handler");
        return new hydroiceutil::LocalStatus( 
            context_.tracer(),
            hydroutil::Properties( context_.properties()->getPropertiesForPrefix("Orca.Status."),"Orca.Status.") );
    }

    // create remote status object and keep a smart pointer to it
    orcaice::detail::StatusI* pobj = new orcaice::detail::StatusI( context_ );
    statusObj_ = pobj;
    // add this object to the adapter and name it 'status'
    context_.adapter()->add( statusObj_, context_.communicator()->stringToIdentity("status") );

    hydroutil::Status* stat = (hydroutil::Status*)pobj;
    orcaice::initTracerInfo( context_.tag()+": Initialized status handler");
    return stat;
}

Home*
Component::initHome()
{
    // We use programmatic configration as default. Config file settings will always overwrite.
    bool enableInterface = interfaceFlag_ & HomeInterface;
    enableInterface = context_.properties()->getPropertyAsIntWithDefault( "Orca.Component.EnableHome", enableInterface );
    // in case the settings have changed, update the flag (it may be used by someone in the future)
    if ( enableInterface )
        interfaceFlag_ = ComponentInterfaceFlag( interfaceFlag_ | HomeInterface );
    else
        interfaceFlag_ = ComponentInterfaceFlag( interfaceFlag_ & ~HomeInterface );

    if ( !enableInterface ) {
        // local object only
        return new orcaice::detail::LocalHome;
    }
    
    // Create the home interface
    orcaice::HomeI* hobj = new orcaice::HomeI( interfaceFlag_, context_ );

    // add the home interface to our adapter
    Ice::ObjectPtr homeObj = hobj;
    std::string homeIdentity = toHomeIdentity( context_.name() );
    homePrx_ = context_.adapter()->add( homeObj, context_.communicator()->stringToIdentity(homeIdentity) );

    orcaice::initTracerInfo( context_.tag()+": Initialized Home interface");
    return (Home*)hobj;
}

hydroutil::History*
Component::initHistory()
{
    hydroutil::History* history = new hydroiceutil::LocalHistory( 
        hydroutil::Properties( context_.properties()->getPropertiesForPrefix("Orca.History."),"Orca.History.") );

    orcaice::initTracerInfo( context_.tag()+": Initialized local history handler");

    return history;
}

void
Component::getNetworkProperties()
{
    // If _anything_ goes wrong, print an error message and throw exception
    try {
        // Connect to the remote properties server
        std::string propertyServerProxyString = orcaice::getPropertyWithDefault( context_.properties(), 
                                                                                 "Orca.PropertyServerProxyString",
                                                                                 "" );
        if ( propertyServerProxyString.empty() )
            return;

        // Get the properties from the remote properties server
        orca::PropertiesPrx propertyPrx;
        orcaice::connectToInterfaceWithString( context(), propertyPrx, propertyServerProxyString );
        orca::PropertiesData propData = propertyPrx->getData();
        const std::map<std::string,std::string> &netProps = propData.properties;

        stringstream ssProps;
        ssProps << "Component::getNetworkProperties(): got network properties: " << toString(propData);
        context_.tracer().debug( ssProps.str(), 3 );

        // Copy them into our properties, without over-writing anything that's already set
        for ( std::map<string,string>::const_iterator it=netProps.begin(); it!=netProps.end(); ++it ) 
        {
            const bool forceTransfer = false;
            const string &fromKey   = it->first;
            const string &fromValue = it->second;
            const string &toKey     = it->first;
//            detail::transferProperty( context_.properties(), fromKey, fromValue, toKey, forceTransfer );
            Ice::PropertiesPtr prop = context_.properties();
            int ret = detail::transferProperty( prop, fromKey, fromValue, toKey, forceTransfer );
            stringstream ss;
            if ( ret == 0 )
            {
                ss << "orcaice::Component::getNetworkProperties(): transferred proeprty '"
                   <<it->first<<"' -> '"<<it->second<<"'";
            }
            else
            {
                ss << "orcaice::Component::getNetworkProperties(): retreived network property '"
                   <<it->first<<"' but did not over-write existing value";
            }
            context_.tracer().debug( ss.str() );
        }
    }
    catch ( const Ice::Exception &e )
    {
        std::stringstream ss; ss << "orcaice::Component::getNetworkProperties(): caught exception: " << e;
        context_.tracer().error( ss.str() );
        throw;
    }
    catch ( const std::exception &e )
    {
        std::stringstream ss; ss << "orcaice::Component::getNetworkProperties(): caught exception: " << e.what();
        context_.tracer().error( ss.str() );
        throw;
    }
    catch ( const std::string &e )
    {
        std::stringstream ss; ss << "orcaice::Component::getNetworkProperties(): caught std::string: " << e;
        context_.tracer().error( ss.str() );
        throw;
    }
    catch ( const char* &e )
    {
        std::stringstream ss; ss << "orcaice::Component::getNetworkProperties(): caught char*: " << e;
        context_.tracer().error( ss.str() );
        throw;
    }
    catch ( ... )
    {
        std::stringstream ss; ss << "orcaice::Component::getNetworkProperties(): caught unknown exception.";
        context_.tracer().error( ss.str() );
        throw;
    }
}

void 
Component::activate()
{
    try
    {
        // This next line is to work around an Ice3.2 bug.
        // See: http://www.zeroc.com/forums/help-center/3266-icegrid-activationtimedout.html#post14380
        context_.communicator()->setDefaultLocator(Ice::LocatorPrx::uncheckedCast(context_.communicator()->getDefaultLocator()->ice_collocationOptimized(false)));
        context_.adapter()->activate();
        context_.tracer().debug( "Adapter activated", 2 );
    }
    catch ( Ice::DNSException& e )
    {
        std::stringstream ss;
        ss << "orcaice::Component: Error while activating Component: "<<e<<".  Check network.";
        context_.tracer().warning( ss.str() );
        throw orcaice::NetworkException( ERROR_INFO, ss.str() );
    }
    catch ( Ice::ConnectionRefusedException& e )
    {
        bool requireRegistry = context_.properties()->getPropertyAsInt( "Orca.RequireRegistry" );
        if ( requireRegistry ) {
            std::stringstream ss; 
            ss<<"orcaice::Component: Error while activating Component: "<<e<<". Check IceGrid Registry.";
            context_.tracer().error( ss.str() );
            context_.tracer().info( "orcaice::Component: You may allow to continue by setting Orca.RequireRegistry=0." );
            throw orcaice::NetworkException( ERROR_INFO, ss.str() );
        }
        else {
            std::stringstream ss; ss<<"orcaice::Component: Failed to register Component("<<e<<"), only direct connections will be possible.";
            context_.tracer().warning( ss.str() );
            context_.tracer().info( "orcaice::Component: You may enforce registration by setting Orca.RequireRegistry=1." );
        }
    }
    catch( const Ice::ObjectAdapterDeactivatedException &e )
    {
        std::stringstream ss;
        ss << "orcaice::Component: Failed to activate component because it's deactivating: " << e;
        context_.tracer().warning( ss.str() );
        throw orcaice::ComponentDeactivatingException( ERROR_INFO, ss.str() );
    }
    catch( const Ice::Exception& e )
    {
        std::stringstream ss; ss<<"orcaice::Component: Failed to activate component: "<<e<<".  Check IceGrid Registry.";
        context_.tracer().warning( ss.str() );
        throw orcaice::NetworkException( ERROR_INFO, ss.str() );
    }
}

const std::string
Component::help( const std::string& executable ) const
{
    // strip possible extension
    std::string exec = hydroutil::basename( executable, true );
    std::string s;
    s += "Standard usage:\n";
    s += "  " + executable + " --help           Prints this help and exists.\n";
    s += "  " + executable + " --version        Prints version info and exists.\n";
    s += "  " + executable + " configfile.cfg   Same as "+executable+" --Orca.Config=configfile.cfg\n";
    s += "  " + executable + "                  Same as "+executable+" --Orca.Config="+exec+".cfg\n";
    return s;
}

} // namespace
