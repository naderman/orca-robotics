/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>

#include "inputthread.h"
#include "network.h"

using namespace std;
using namespace teleop;

InputThread::InputThread( Network* network, const orcaice::Context& context ) :
    network_(network),
    driver_(0),
    driverFactory_(0),
    driverLib_(0),
    context_(context)
{
}

InputThread::~InputThread()
{
    delete driver_;
    delete driverFactory_;    
    delete driverLib_;
}

void
InputThread::walk()
{
    //
    // Read settings
    //
    Ice::PropertiesPtr prop = context_.properties();
    std::string prefix = context_.tag() + ".Config.";

    context_.tracer()->debug( "InputThread: Loading driver library.", 4 );
    std::string driverLibName = 
        orcaice::getPropertyWithDefault( prop, prefix+"InputDriverLib", "libHydroHumanInput2dKbdTermio.so" );
    try {
        // Dynamically load the driver from its library
        driverLib_ = new hydrodll::DynamicallyLoadedLibrary(driverLibName);
        driverFactory_ = 
            hydrodll::dynamicallyLoadClass<hydrointerfaces::HumanInput2dFactory,DriverFactoryMakerFunc>
            ( *driverLib_, "createDriverFactory" );
    }
    catch (hydrodll::DynamicLoadException &e)
    {
        context_.tracer()->error( e.what() );
        throw;
    }

    hydroutil::Properties props(
        context_.properties()->getPropertiesForPrefix(prefix), prefix );
    hydrointerfaces::Context driverContext( props, context_.tracer(), context_.status() );
    try {
        context_.tracer()->info( "InputThread: Initialising driver..." );
        driver_ = driverFactory_->createDriver( driverContext );
    }
    catch ( ... )
    {
        stringstream ss;
        ss << "MainThread: Caught unknown exception while initialising driver";
        context_.tracer()->error( ss.str() );
        throw;
    }  

    // don't forget to enable the driver, but check !isStopping() to see if we should quit
    while ( driver_->enable() && !isStopping() ) {
        context_.tracer()->warning("Failed to enable input driver. Will try again in 2 seconds.");
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(2));
    }

    // check again to make sure we are not being terminated
    if ( !!isStopping() ) {
        return;
    }
    context_.tracer()->debug("InputThread: Input driver enabled",2);

    // temp object
    hydrointerfaces::HumanInput2d::Command command;

    //
    // Main loop
    //
    while ( !isStopping() )
    {
        context_.tracer()->debug( "InputThread: calling driver_->read()", 10 );
        if ( !driver_->read( command ) ) {
            network_->newMixedCommand( command );
        }
        else {
            string errString = "failed to read from input hardware";
            throw hydroutil::HardwareException( ERROR_INFO, errString );
        }
    } // end of main loop

    // reset the hardware
    if ( driver_->disable() ) {
        context_.tracer()->warning("Failed to disable input driver");
    }
    context_.tracer()->debug("Input driver disabled",2);
}
