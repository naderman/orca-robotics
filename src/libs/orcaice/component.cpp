/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#include <string>
#include <orca/common.h>
#include <orcaice/orcaice.h>
#include <hydroiceutil/localhistory.h>

#include "component.h"
#include "detail/privateutils.h"
#include "detail/componentthread.h"
#include "detail/privateutils.h"
#include "detail/homeImpl.h"
#include "detail/statusImpl.h"
#include "detail/tracerImpl.h"

// debug only
#include <iostream>
using namespace std;

namespace orcaice {

Component::Component( const std::string& tag, ComponentInterfaceFlag interfaceFlag_ ) :
    interfaceFlag_(interfaceFlag_)
{
    context_.tag_ = tag;
}

//
// IMPORTANT! This destructor must be here (in the .cpp file)
// Otherwise, the destructors of the forward-declared types will not be called.
//
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
    context_.init( name, isApp, adapter ); //, this );

    Ice::PropertiesPtr props = context_.properties();

    // Decide which standard interfaces we'll activate by combining
    //  a) programmer's defaults encoded in ComponentInterfaceFlag, and
    //  b) user preferences which are (optionally) set in configs
    // We update the properties to our final decision, so that we act accordingly later.

    {
        bool enable = interfaceFlag_ & HomeInterface;
        enable = props->getPropertyAsIntWithDefault( "Orca.Component.EnableHome", enable );
        props->setProperty( "Orca.Component.EnableHome", (enable ? "1" : "0") );
    }
    {
        bool enable = interfaceFlag_ & StatusInterface;
        enable = props->getPropertyAsIntWithDefault( "Orca.Component.EnableStatus", enable );
        props->setProperty( "Orca.Component.EnableStatus", (enable ? "1" : "0") );
    }
    {
        bool enable = interfaceFlag_ & TracerInterface;
        enable = props->getPropertyAsIntWithDefault( "Orca.Component.EnableTracer", enable );
        props->setProperty( "Orca.Component.EnableTracer", (enable ? "1" : "0") );
    }

    //
    // Create component services
    // Must be careful about the order in which they are created.
    // After all of them are created, we'll have to give them the updated context.
    //
    // must be created first. does not rely on other services but needs the component name.
    tracer_.reset( new detail::TracerImpl( context_ ) );
    context_.tracer_ = (gbxutilacfr::Tracer*)tracer_.get();
    initTracerInfo( context_.tag() + ": Component service Tracer created." );

    // must created after tracer. needs tracer and component name.
    status_.reset( new detail::StatusImpl( context_ ) );
    context_.status_ = (gbxutilacfr::Status*)status_.get();
    initTracerInfo( context_.tag() + ": Component service Status created." );

    // can be created last needs component name.
    home_.reset( new detail::HomeImpl( context_ ) );
    context_.home_ = (orcaice::Home*)home_.get();
    initTracerInfo( context_.tag() + ": Component service Home created." );

    // now all services are in the context, update it.
    tracer_->updateContext( context_ );
    status_->updateContext( context_ );
    home_->updateContext( context_ );

    //
    // Initialize Tracer first so that it can be used as soon as possible.
    // (services need the context the 2nd time because now it's fully populated -- with them)
    //
    if ( props->getPropertyAsInt( "Orca.Component.EnableTracer" ) ) {
        tracer_->initInterface();
        initTracerInfo( context_.tag() + ": Tracer interface initialized." );
    }

    if ( props->getPropertyAsInt( "Orca.Component.EnableStatus" ) ) {
        status_->initInterface();
        initTracerInfo( context_.tag() + ": Status interface initialized." );
    }
        
    if ( props->getPropertyAsInt( "Orca.Component.EnableHome" ) ) {
        home_->initInterface();
        initTracerInfo( context_.tag() + ": Home interface initialized." );
    }

    //
    // Get config properties from a central server
    //
    detail::setComponentPropertiesFromServer( context_ );

    // the last component service History, create after all properties were received
    hydroutil::Properties historyProps( context_.properties()->getPropertiesForPrefix("Orca.History."),"Orca.History.");
    // If DefaultFilename is not specified, add one based on our tag name (i.e. component name)
    if ( !historyProps.isDefined( "DefaultFilename" ) )
        historyProps.setProperty( "DefaultFilename", hydroutil::toLowerCase( context_.tag()+"-history.txt" ) );
    history_.reset( new hydroiceutil::LocalHistory( historyProps ) );
    context_.history_ = history_.get();
    initTracerInfo( context_.tag() + ": Component service History created." );

    //
    // create infrastructure thead
    //
    componentThread_ = new detail::ComponentThread( context_ );
    try {
        componentThread_->start();
    }
    catch ( ... )
    {
        orcaice::catchExceptions( context_.tracer(), "starting component utility thread" );
        context_.shutdown();
    }
    initTracerInfo( context_.tag() + ": Component infrastructure thread created." );
};

void
Component::finalise()
{
    if ( componentThread_ )
    {
        context_.tracer().debug( "orcaice::Component: stopping ComponentThread....", 2 );
        gbxiceutilacfr::stopAndJoin( componentThread_ );
        context_.tracer().debug( "orcaice::Component: ComponentThread stopped.", 2 );
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
