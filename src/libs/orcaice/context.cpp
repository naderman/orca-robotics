/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include "context.h"
#include <Ice/Ice.h>
#include <gbxutilacfr/exceptions.h>
#include <orcaice/exceptions.h>

namespace orcaice {

Context::Context() :
    home_(0),
    tracer_(0),
    status_(0),
    history_(0)
{
}

void 
Context::init( const orca::FQComponentName &name,
               const bool                   isApp,
               const Ice::ObjectAdapterPtr &adapter )
{
    name_ = name;
    isApplication_ = isApp;
    adapter_ = adapter;

    communicator_ = adapter_->getCommunicator();
};

void
Context::activate()
{
    try
    {
        // This next line was to work around an Ice3.2 bug.
        // See: http://www.zeroc.com/forums/help-center/3266-icegrid-activationtimedout.html#post14380
        communicator_->setDefaultLocator(Ice::LocatorPrx::uncheckedCast(communicator_->getDefaultLocator()->ice_collocationOptimized(false)));

        adapter_->activate();
        tracer_->debug( "Adapter activated", 2 );
    }
    catch ( Ice::DNSException& e )
    {
        std::stringstream ss;
        ss << "(while activating orcaice::Component) \n"<<e<<"\nCheck network.";
//         tracer_->warning( ss.str() );
        throw orcaice::NetworkException( ERROR_INFO, ss.str() );
    }
    catch ( Ice::ConnectionRefusedException& e )
    {
        bool requireRegistry = properties()->getPropertyAsInt( "Orca.Component.RequireRegistry" );
        if ( requireRegistry ) {
            std::stringstream ss; 
            ss<<"(while activating orcaice::Component) failed: \n"<<e<<"\nCheck IceGrid Registry.";
//             tracer_->error( ss.str() );
            ss<<"\nYou may allow to continue by setting Orca.Component.RequireRegistry=0.";
            throw orcaice::NetworkException( ERROR_INFO, ss.str() );
        }
        else {
            std::stringstream ss; 
            ss<<"(while activating orcaice::Component) failed:\n"<<e;
            tracer_->warning( ss.str() );
            tracer_->info( "Continuing, but only direct outgoing connections will be possible." );
            tracer_->info( "You may enforce registration by setting Orca.Component.RequireRegistry=1." );
        }
    }
    catch ( Ice::ConnectFailedException& e )
    {
        bool requireRegistry = properties()->getPropertyAsInt( "Orca.Component.RequireRegistry" );
        if ( requireRegistry ) {
            std::stringstream ss; 
            ss<<"(while activating orcaice::Component) failed: \n"<<e<<"\nCheck IceGrid Registry.";
//             tracer_->error( ss.str() );
            ss<<"\nYou may allow to continue by setting Orca.Component.RequireRegistry=0.";
            throw orcaice::NetworkException( ERROR_INFO, ss.str() );
        }
        else {
            std::stringstream ss; 
            ss<<"(while activating orcaice::Component) failed:\n"<<e;
            tracer_->warning( ss.str() );
            tracer_->info( "Continuing, but only direct outgoing connections will be possible." );
            tracer_->info( "You may enforce registration by setting Orca.Component.RequireRegistry=1." );
        }
    }
    catch( const Ice::ObjectAdapterDeactivatedException& e )
    {
        std::stringstream ss;
        ss << "(while activating orcaice::Component) failed: component is deactivating: " << e;
//         tracer_->warning( ss.str() );
        throw orcaice::ComponentDeactivatingException( ERROR_INFO, ss.str() );
    }
    catch( const Ice::Exception& e )
    {
        std::stringstream ss; 
        ss<<"orcaice::Component: Failed to activate component: "<<e<<"\nCheck IceGrid Registry.";
//         tracer_->warning( ss.str() );
        throw orcaice::NetworkException( ERROR_INFO, ss.str() );
    }
}

void
Context::shutdown() const
{
    if ( !communicator_ )
        throw gbxutilacfr::Exception( ERROR_INFO, "Trying to shutdown component before context initializition." );

    if ( isApplication() ) {
        tracer().info( "Triggering component shutdown ...");
        communicator()->shutdown();
    }
    else {
        tracer().info( "NOT triggering component shutdown because the component is running as an IceBox service ...");
    }
}

bool 
Context::isDeactivating()
{
    try {
        adapter()->getCommunicator();
        return false;
    } 
    catch (const Ice::ObjectAdapterDeactivatedException&) {
        return true;
    }
}

hydroutil::Context 
Context::toHydroContext() const
{
    return toHydroContext( tag()+".Config." );
}

hydroutil::Context 
Context::toHydroContext( const std::string &prefixToStrip ) const
{
    // transfer Orca properties into Hydro configs
    hydroutil::Properties::Config config;
    if ( properties()->getPropertyAsIntWithDefault( "Orca.Warn.DefaultProperty", 1 ) )
        config.warnDefaultProperty = true;

    return hydroutil::Context( hydroutil::Properties( properties()->getPropertiesForPrefix(prefixToStrip), prefixToStrip, config ), 
                               tracer(), 
                               status() );
}

std::string 
Context::toString() const
{
    std::stringstream ss;
    ss<<"tag="<<tag();
    ss<<"\nplatform="<<name().platform;
    ss<<"\ncomponent="<<name().component;
    ss<<"\nisApplication="<<(int)isApplication();
    ss<<"\ncommunicator="<<(int)(communicator()!=0);
    ss<<"\nadapter="<<(int)(adapter()!=0);
    ss<<"\nproperties=("<<properties()->getCommandLineOptions().size()<<")";

    return ss.str();
}   

} // namespace
