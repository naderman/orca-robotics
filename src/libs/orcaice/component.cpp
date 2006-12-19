/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
 
#include <IceStorm/IceStorm.h> // used in initTracer()
#include <IceGrid/Admin.h>     // used in initHome() to register Home interface as a well-known object
#include <string>

#include <orca/orca.h>
#include <orcaice/orcaice.h>

#include "homeI.h"
#include "statusI.h"
#include "tracerI.h"
#include "privateutils.h"

#include "component.h"

// debug only
#include <iostream>

namespace orcaice {


Component::Component( const std::string &tag, ComponentInterfaceFlag flag )
    : interfaceFlag_(flag)
{
    context_.tag_ = tag;
}

void
Component::init( const orca::FQComponentName & name,
                 const bool isApp,
                 const Ice::ObjectAdapterPtr & adapter )
{
    // set context with component info
    // this is the only storage of this info
    context_.init( name, isApp, adapter, this );

    //
    // Initialize tracing object
    //
    context_.tracer_ = initTracer();

    //
    // Optionally, initialize Status interface
    //
    if ( interfaceFlag_ & StatusInterface ) {
        context_.status_ = initStatus();
    }

    //
    // Optionally, initialize Home interface
    //
    if ( interfaceFlag_ & HomeInterface ) {
        initHome();
    }
};

Tracer*
Component::initTracer()
{
    orcaice::initTracerPrint( tag()+": Initializing application trace handler...");
        
    // this is a bit tricky. we need
    // 1. a smart pointer which derives from Object (or ObjectPtr itself) to add to adapter
    // 2. a smart pointer which derives from Tracer to save in context
    // Ideally we'd have something like StatusTracerPtr which does derive from both.
    // but the smart pointer stuff is then included twice and reference counters get confused.
    // So first we use the pointer to orcaice::StatusTracerI, then change to Ice::ObjectPtr and Tracer*.
    orcaice::detail::TracerI* pobj = new orcaice::detail::TracerI( context_ );
    Ice::ObjectPtr obj = pobj;
    //TracerPtr trac = pobj;
    // have to revert to using plain pointers. Otherwise, we get segfault on shutdown when
    // trac tries to delete the object which already doesn't exist. Something wrong with ref counters.
    Tracer* trac = (Tracer*)pobj;
    
    //
    // add this object to the adapter and name it 'tracer'
    // 
    context_.adapter()->add( obj, context_.communicator()->stringToIdentity("tracer") );

    // a bit of a hack: keep this smart pointer so it's not destroyed with the adapter
    tracerObj_ = obj;
    
    initTracerPrint( tag()+": Tracer initialized" );
    return trac;
}

Status*
Component::initStatus()
{
    orcaice::initTracerPrint( tag()+": Initializing application status handler ...");
        
    // this is a bit tricky. we need
    // 1. a smart pointer which derives from Object (or ObjectPtr itself) to add to adapter
    // 2. a smart pointer which derives from Tracer to save in context
    // Ideally we'd have something like StatusTracerPtr which does derive from both.
    // but the smart pointer stuff is then included twice and reference counters get confused.
    // So first we use the pointer to orcaice::StatusTracerI, then change to Ice::ObjectPtr and Tracer*.
    orcaice::detail::StatusI* pobj = new orcaice::detail::StatusI( context_ );
    Ice::ObjectPtr obj = pobj;
    //TracerPtr trac = pobj;
    // have to revert to using plain pointers. Otherwise, we get segfault on shutdown when
    // trac tries to delete the object which already doesn't exist. Something wrong with ref counters.
    Status* stat = (Status*)pobj;
    
    //
    // add this object to the adapter and name it 'status'
    // 
    context_.adapter()->add( obj, context_.communicator()->stringToIdentity("status") );
    // a bit of a hack: keep a smart pointer so it's not destroyed with the adapter
    statusObj_ = obj;
    
    initTracerPrint( tag()+": Status initialized" );

    return stat;
}

void
Component::initHome()
{
    orca::ComponentData compData;
    compData.name = context_.name();

    // parse the configuration file to get provided and required interfaces.
    // interfaces which are not in the config file will not be visible through Home.
    Ice::PropertyDict providesProps =
        context_.properties()->getPropertiesForPrefix(tag()+".Provides");
    orca::ProvidedInterface provided;
    for ( Ice::PropertyDict::iterator i=providesProps.begin(); i!=providesProps.end(); ++i ) {
        provided.name = i->second;
        provided.id = "unknown";
        compData.provides.push_back( provided );
    }
    // special cases: add standard interfaces (depending on startup flags)
    // first, add the Home interface itself 
    std::string homeIdentity = "orca." + context_.name().platform + "." + context_.name().component + "/Home";
    provided.name = homeIdentity;
    provided.id = "::orca::Home";
    compData.provides.push_back( provided );
    if ( interfaceFlag_ & StatusInterface ) {
        provided.name = "status";
        provided.id = "::orca::Status";
        compData.provides.push_back( provided );
    }

    // NOTE: this will not work if the config file uses long notation for req. interfaces
    Ice::PropertyDict requiresProps =
        context_.properties()->getPropertiesForPrefix(tag()+".Requires");
    orca::RequiredInterface required;
    for ( Ice::PropertyDict::iterator i=requiresProps.begin(); i!=requiresProps.end(); ++i ) {
        required.name = orcaice::toInterfaceName( i->second );
        required.id = "unknown";
        compData.requires.push_back( required );
    }
        
    // PROVIDED INTERFACE: Home
    Ice::ObjectPtr homeObj = new orcaice::HomeI( compData, context_.properties()->getPropertiesForPrefix("") );
//     context_.adapter()->add( homeObj, context_.communicator()->stringToIdentity("home") );

    // make Home a well-known object
    Ice::ObjectPrx homePrx =context_.adapter()->add( homeObj, context_.communicator()->stringToIdentity(homeIdentity) );

    Ice::ObjectPrx adminPrx = context_.communicator()->stringToProxy("IceGrid/Admin");
//             orcamisc::stringToIceGridInstanceName(locatorString)+"/Admin");
    
    IceGrid::AdminPrx admin;
    try {
        admin = IceGrid::AdminPrx::checkedCast( adminPrx );
        admin->addObjectWithType( homePrx, "::orca::Home" );
    } 
    catch (const IceGrid::ObjectExistsException &) {
        admin->updateObject( homePrx );
    }
    catch ( Ice::Exception &e ) {
        bool requireRegistry = properties()->getPropertyAsInt( "Orca.RequireRegistry" );
        if ( requireRegistry ) {
            std::stringstream ss;
            ss << "Failed to register Home interface: "<<e<<".";
            tracer()->error( ss.str()+std::string("  Check IceGrid Registry.") );
            throw orcaice::NetworkException( ERROR_INFO, ss.str()+std::string("You may allow things to continue without registration by setting Orca.RequireRegistry=0.") );
        }
        else {
            std::stringstream ss;
            ss << "Failed to register Home interface: "<<e<<".";
            tracer()->warning( ss.str() );
            tracer()->info( "You may enforce registration by setting Orca.RequireRegistry=1." );
        }
    }
}

void 
Component::activate()
{
    try
    {
        context_.adapter()->activate();
        tracer()->debug( "adapter activated", 5 );
    }
    catch ( Ice::DNSException &e )
    {
        std::stringstream ss;
        ss << "Error while activating Component: "<<e<<".  Check network.";
        tracer()->warning( ss.str() );
        throw orcaice::NetworkException( ERROR_INFO, ss.str() );
    }
    catch ( Ice::ConnectionRefusedException &e )
    {
        bool requireRegistry = properties()->getPropertyAsInt( "Orca.RequireRegistry" );
        if ( requireRegistry ) {
            std::stringstream ss; 
            ss<<"Error while activating Component: "<<e<<". Check IceGrid Registry.";
            tracer()->error( ss.str() );
            tracer()->info( "You may allow to continue by setting Orca.RequireRegistry=0." );
            throw orcaice::NetworkException( ERROR_INFO, ss.str() );
        }
        else {
            std::stringstream ss; ss<<"Failed to register Component("<<e<<"), only direct connections will be possible.";
            tracer()->warning( ss.str() );
            tracer()->info( "You may enforce registration by setting Orca.RequireRegistry=1." );
        }
    }
    catch( const Ice::Exception &e )
    {
        std::stringstream ss; ss<<"Failed to activate component: "<<e<<".  Check IceGrid Registry.";
        tracer()->warning( ss.str() );
        throw orcaice::NetworkException( ERROR_INFO, ss.str() );
    }
}

std::string
Component::help()
{
    std::string s;
    s += "Standard usage:\n";
    s += "\t--help\n";
    s += "\t--version\n";
    return s;
}

} // namespace
