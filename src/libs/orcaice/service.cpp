/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <orcaice/service.h>
#include <orcaice/component.h>

#include <orca/orca.h>
#include <orcaice/orcaice.h>

#include "privateutils.h"


using namespace std;

namespace orcaice
{

namespace {

void setProperties( Ice::PropertiesPtr         &properties,
                    const Ice::StringSeq       &commandLineArgs,
                    const std::string          &componentTag,
                    const Ice::CommunicatorPtr &communicator,
                    bool                       &isSharedCommunicatorConfigured )
{
    // pre-parse Orca-specific command line arguments
    // (nothing here right now)
    // orcaice::parseOrcaCommandLineOptions( args );

    // Level 4. Highest priority, apply properties from the command line arguments
    // read in all command line options starting with '--", but not "-"
    // note that something like --bullshit will be parsed to --bullshit=1
    // Note that this is a standard Ice function.
    properties->parseCommandLineOptions( "", commandLineArgs );
    initTracerInfo( componentTag+": Loaded command line properties" );

    // Level 3. Now, apply properties from this component's config file (do not force!)
    // Note that unlike in Application, it's possible that all required properties are given
    // in the icebox.cfg file and the component config file is not specified.
    detail::addPropertiesFromServiceConfigFile( properties, commandLineArgs, componentTag );

    // (alexb: can we check this outside the function, before calling setProperties()?)
    // Apply factory defaults and global settings only if
    //   a) this is private communicator (not shared with others), or
    //   b) the communicator is shared but not yet configured
    // The end result is that both private and shared communicators are only
    // configured once.
    bool applyFactoryAndGlobals = false;
    if ( !properties->getPropertyAsInt( componentTag+".UseSharedCommunicator" ) ) {
        applyFactoryAndGlobals = true;
        // debug
        initTracerInfo( componentTag+": Will apply factory and global settings (private communicator).");
    }
    else if ( !isSharedCommunicatorConfigured ) {
        applyFactoryAndGlobals = true;
        isSharedCommunicatorConfigured = true;
        // debug
        initTracerInfo( componentTag+": Will apply factory and global settings (shared communicator, 1st service).");
    }

    if ( applyFactoryAndGlobals ) 
    {
        // Level 2. Now, apply properties from the global Orca config file
        detail::addPropertiesFromGlobalConfigFile( properties, componentTag );

        // Level 1. apply Orca factory defaults
        orcaice::detail::setFactoryProperties( properties, componentTag );
        initTracerInfo( componentTag+": Loaded factory default properties." );

        // Level 0. extra funky stuff
        // if we can, set some of communicator properties "after the fact"
        // (the communicator has already been started and will not reload its confg parameters now,
        // but some things can be changed and will be used in the future).
        std::string defaultLocatorStr = properties->getProperty( "Ice.Default.Locator" );
        Ice::LocatorPrx defaultLocator = Ice::LocatorPrx::uncheckedCast( communicator->stringToProxy( defaultLocatorStr ) );
        communicator->setDefaultLocator( defaultLocator );
    }
}

}

bool Service::_isSharedCommunicatorConfigured = false;

Service::Service()
    : component_(0)
{
}

Service::~Service()
{
    delete component_;
}

void
Service::start( const ::std::string        & name,
                const Ice::CommunicatorPtr & communicator,
                const Ice::StringSeq       & args )
{    
    if ( component_==0 ) {
        initTracerError( "service has no component to run!" );
        return;
    }

    // debug
//     cout<<args.size()<<" args :"<<orcaice::toString( args )<<endl;

    // print version information on the first line
    orcaice::detail::printAllVersions( *component_ );

    // Unlike in Application, the component tag may change when used with Service. This is to 
    // allow multiple components of the same type to be placed inside the same IceBox. The
    // actual tag is given by to us by IceBox as 'name'.
    component_->setTag( name );
    initTracerInfo("Reset component tag to "+name);

    // Unlike in Application, the args here do NOT include the executable name

    // Unlike in Application which runs before the Communicator is created, here it already
    // exists and the pointer to it is given to us by the IceBox. So we just get a pointer
    // to the properties which also already exist.
    Ice::PropertiesPtr properties = communicator->getProperties();

    // Set the component's properties based on the various sources from which properties can be read
    setProperties( properties,
                   args,
                   component_->tag(),
                   communicator,
                   _isSharedCommunicatorConfigured );

    // now communicator exists. we can further parse properties, make sure all the info is
    // there and set some properties (notably AdapterID)
    orca::FQComponentName fqCompName =
                orcaice::detail::parseComponentProperties( communicator, component_->tag() );

    // print all prop's now, after some stuff was added, e.g. Tag.AdapterId
    // note: is it possible that some of the prop's got stripped off by Ice::Application::main()? I don't think so.
    if ( properties->getPropertyAsInt( "Orca.PrintProperties" ) ) {
        orcaice::detail::printComponentProperties( properties, component_->tag() );
    }

    // create the one-and-only component adapter
    adapter_ = communicator->createObjectAdapter( component_->tag() );
    initTracerInfo( component_->tag()+": Created object adapter" );

    //
    // Give the component all the stuff it needs
    //
    bool isApp = false;
    component_->init( fqCompName, isApp, adapter_ );
    initTracerInfo( component_->tag()+": Service initialized" );

    //
    // Start the component, catching all exceptions
    //
    stringstream exceptionSS;
    try
    {
        component_->start();
        if ( communicator->getProperties()->getPropertyAsInt( "Orca.PrintComponentStarted" ) ) {
            initTracerInfo( component_->tag()+": Component started" );
        }
    }
    catch ( const Ice::Exception &e ) {
        exceptionSS << "Caught exception while starting component : " << e;
    }
    catch ( const std::exception &e ) {
        exceptionSS << "Caught exception while starting component: " << e.what();
    }
    catch ( const std::string &e ) {
        exceptionSS << "Caught exception while starting component: " << e;
    }
    catch ( const char *e ) {
        exceptionSS << "Caught exception while starting component: " << e;
    }
    catch ( ... ) {
        exceptionSS << "Caught exception while starting component.";
    }

    if ( !exceptionSS.str().empty() ) {
        initTracerError( component_->tag()+": "+exceptionSS.str() );
        initTracerInfo( component_->tag()+": Service quitting." );
    }
}

void
Service::stop()
{

    if ( component_ )
    {
        initTracerInfo( component_->tag()+": Stopping service..." );
        component_->finalise();
        component_->stop();
        initTracerInfo( component_->tag()+": Component stopped" );
    }

    adapter_->waitForDeactivate();
    initTracerInfo( component_->tag()+": Adapter deactivated" );

    adapter_ = 0;
    initTracerInfo( component_->tag()+": Service stopped." );
}

} // namespace
