/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2008 Tom Burdick, Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 * 
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include "mainthread.h"

#include <iostream>
#include <memory>
#include <orcaice/orcaice.h>
#include <orcaobj/orcaobj.h> // for getPropertyAs...()

using namespace std;
using namespace disparity;

MainThread::MainThread( const orcaice::Context &context ) :
    orcaice::SubsystemThread( context.tracer(), context.status(), "MainThread" ),
    context_(context)
{
}

void
MainThread::initialise()
{
    readSettings();

    initPluginInterface();

    initNetworkInterface();
}

void
MainThread::work() 
{
    subStatus().setHeartbeatInterval( 1.0 );

    while( !isStopping() )
    {
        try
        {
            //read data in
            
            //process it

            //push processed data out
        }
        catch( ... )
        {
        }
    }
}

void
MainThread::finalise() 
{
}

////////////////////

void
MainThread::readSettings()
{
    // read in settings from a config file
    Ice::PropertiesPtr prop = context_.properties();
    std::string prefix = context_.tag() + ".Config.";

    

    //connect with a proxy and fetch the incoming description object
    orcaice::connectToInterfaceWithTag<orca::MultiCameraPrx>( context_
            , incomingPrx_
            , "MultiCamera"
            , this
            , subsysName()
            );

    incomingDescr_ = incomingPrx_->getDescription();

    context_.tracer().info( orcaobj::toString( incomingDescr_ ) );

}

void
MainThread::initNetworkInterface()
{
    active( context_, this, subsysName() );

    // incoming network interface
    incomingInterface_ = new orcaifaceimpl::BufferedMultiCameraConsumerImpl( 1
            , gbxiceutilacfr::BufferTypeCircular
            , context_ 
            );

    incomingInterface_->subscribeWithTag( "MultiCamera"
            , this
            , subsysName() 
            );

    // outgoing network interface
    outgoingInterface_ = new orcaifaceimpl::ImageImpl( outgoingDescr_
        , "Disparity"
        , context_ );

    outgoingInterface_->initInterface(this, subsysName() );

}

void 
MainThread::initPluginInterface()
{
}


