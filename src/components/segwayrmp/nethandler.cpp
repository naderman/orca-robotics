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
#include <sstream>

#include <orcaice/orcaice.h>
#include "nethandler.h"

// implementations of Ice objects
#include <orcaifaceimpl/odometry2dI.h>
#include <orcaifaceimpl/odometry3dI.h>
#include <orcaifaceimpl/powerI.h>
#include "velocitycontrol2dI.h"

using namespace std;
using namespace segwayrmp;

NetHandler::NetHandler(
                 orcaice::Proxy<orca::Odometry2dData>&      odometry2dPipe,
                 orcaice::Proxy<orca::Odometry3dData>&      odometry3dPipe,
                 orcaice::Notify<orca::VelocityControl2dData>&  commandPipe,
                 orcaice::Proxy<orca::PowerData>&           powerPipe,
                 const orcaice::Context&                    context ) :
    odometry2dPipe_(odometry2dPipe),
    odometry3dPipe_(odometry3dPipe),
    commandPipe_(commandPipe),
    powerPipe_(powerPipe),
    context_(context)
{
}

NetHandler::~NetHandler()
{
}

void
NetHandler::run()
{
    try // this is once per run try/catch: waiting for the communicator to be destroyed
    {

    while ( isActive() )
    {
        try {
            context_.activate();
            break;
        }
        catch ( orcaice::NetworkException & e )
        {
            std::stringstream ss;
            ss << "nethandler::run: Caught NetworkException: " << e.what() << endl << "Will try again...";
            context_.tracer()->warning( ss.str() );
        }
        catch ( Ice::Exception & e )
        {
            std::stringstream ss;
            ss << "nethandler::run: Caught Ice::Exception while activating: " << e << endl << "Will try again...";
            context_.tracer()->warning( ss.str() );
        }
        catch ( ... )
        {
            cout << "Caught some other exception while activating." << endl;
        }
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(2));
    }

    std::string prefix = context_.tag() + ".Config.";

    //
    // PROVIDED: VelocityControl2d
    //
    orca::VelocityControl2dDescription velocityControl2dDescr;

    velocityControl2dDescr.maxVelocities.v.x = 
            orcaice::getPropertyAsDoubleWithDefault( context_.properties(), prefix+"MaxSpeed", 1.0 );
    velocityControl2dDescr.maxVelocities.v.y = 0.0;
    velocityControl2dDescr.maxVelocities.w = 
            orcaice::getPropertyAsDoubleWithDefault( context_.properties(), prefix+"MaxTurnRate", 45.0 );

    // create servant for direct connections and tell adapter about it
    // don't need to store it as a member variable, adapter will keep it alive
    Ice::ObjectPtr velocityControl2dI = new VelocityControl2dI( velocityControl2dDescr, commandPipe_ );

    // two possible exceptions will kill it here, that's what we want
    orcaice::createInterfaceWithTag( context_, velocityControl2dI, "VelocityControl2d" );

    //
    // PROVIDED: Odometry2d
    //
    orca::Odometry2dDescription odometry2dDescr;

    orcaice::setInit( odometry2dDescr.offset );
    orcaice::getPropertyAsFrame2d( context_.properties(), prefix+"Offset", odometry2dDescr.offset );
    orcaice::setInit( odometry2dDescr.size, 1.0, 1.0 );
    orcaice::getPropertyAsSize2d( context_.properties(), prefix+"Size", odometry2dDescr.size );

    orcaifaceimpl::Odometry2dIPtr odometry2dI = 
            new orcaifaceimpl::Odometry2dI( odometry2dDescr, "Odometry2d", context_ );


    while ( isActive() ) {
        try {
            odometry2dI->initInterface();
            context_.tracer()->debug( "odometry 2d interface initialized",2);
            break;
        }
        catch ( const orcaice::NetworkException& e ) {
            context_.tracer()->warning( "Failed to setup interface. Check Registry and IceStorm. Will try again in 2 secs...");
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(2));
        }
        catch ( const Ice::Exception& e ) {
            context_.tracer()->warning( "Failed to setup interface. Check Registry and IceStorm. Will try again in 2 secs...");
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(2));
        }
    }

    //
    // PROVIDED: Odometry3d
    //
    orca::Odometry3dDescription odometry3dDescr;

    orcaice::setInit( odometry3dDescr.offset );
    orcaice::getPropertyAsFrame3d( context_.properties(), prefix+"Offset", odometry3dDescr.offset );
    orcaice::setInit( odometry3dDescr.size, 1.0, 1.0, 2.0 );
    orcaice::getPropertyAsSize3d( context_.properties(), prefix+"Size", odometry3dDescr.size );

    orcaifaceimpl::Odometry3dIPtr odometry3dI = 
            new orcaifaceimpl::Odometry3dI( odometry3dDescr, "Odometry3d", context_ );


    while ( isActive() ) {
        try {
            odometry3dI->initInterface();
            context_.tracer()->debug( "odometry 3d interface initialized",2);
            break;
        }
        catch ( const orcaice::NetworkException& e ) {
            context_.tracer()->warning( "Failed to setup interface. Check Registry and IceStorm. Will try again in 2 secs...");
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(2));
        }
        catch ( const Ice::Exception& e ) {
            context_.tracer()->warning( "Failed to setup interface. Check Registry and IceStorm. Will try again in 2 secs...");
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(2));
        }
    }

    //
    // PROVIDED: Power
    //

    orcaifaceimpl::PowerIPtr powerI = new orcaifaceimpl::PowerI( "Power", context_ );


    while ( isActive() ) {
        try {
            powerI->initInterface();
            context_.tracer()->debug( "power interface initialized",2);
            break;
        }
        catch ( const orcaice::NetworkException& e ) {
            context_.tracer()->warning( "Failed to setup interface. Check Registry and IceStorm. Will try again in 2 secs...");
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(2));
        }
        catch ( const Ice::Exception& e ) {
            context_.tracer()->warning( "Failed to setup interface. Check Registry and IceStorm. Will try again in 2 secs...");
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(2));
        }
    }


    orca::Odometry2dData odometry2dData;
    orca::Odometry3dData odometry3dData;
    orca::PowerData powerData;

    orcaice::Timer odometry2dPublishTimer;
    orcaice::Timer odometry3dPublishTimer;
    orcaice::Timer powerPublishTimer;

    double odometry2dPublishInterval = orcaice::getPropertyAsDoubleWithDefault( context_.properties(),
            prefix+"Position2dPublishInterval", -1 );
    double odometry3dPublishInterval = orcaice::getPropertyAsDoubleWithDefault( context_.properties(),
            prefix+"Position3dPublishInterval", -1 );
    double powerPublishInterval = orcaice::getPropertyAsDoubleWithDefault( context_.properties(),
            prefix+"PowerPublishInterval", 20.0 );
//     double statusPublishInterval = orcaice::getPropertyAsDoubleWithDefault( context_.properties(),
//             prefix+"StatusPublishInterval", 60.0 );

    const int odometryReadTimeout = 500; // [ms]

    //
    // Main loop
    //
    while( isActive() )
    {
//         context_.tracer()->debug( "net handler loop spinning ",1);

        // block on the most frequent data source: odometry
        if ( odometry2dPipe_.getNext( odometry2dData, odometryReadTimeout ) ) {
//             context_.tracer()->debug( "Net loop timed out", 1);
            continue;
        }

        // check that we were not told to terminate while we were sleeping
        // otherwise, we'll get segfault (there's probably a way to prevent this inside the library)
        if ( isActive() && odometry2dPublishTimer.elapsed().toSecondsDouble()>=odometry2dPublishInterval ) {
            odometry2dI->localSetAndSend( odometry2dData );
            odometry2dPublishTimer.restart();
        } 
        else {
            odometry2dI->localSet( odometry2dData );
        }

        // now send less frequent updates
        try
        {
            if ( isActive() && odometry3dPipe_.isNewData() ) 
            {
                if ( odometry3dPublishTimer.elapsed().toSecondsDouble()>=odometry3dPublishInterval ) {
                    odometry3dPipe_.get( odometry3dData );
                    odometry3dI->localSetAndSend( odometry3dData );
                    odometry3dPublishTimer.restart();
                }
                else {
                    odometry3dI->localSet( odometry3dData );
                }
            }
            if ( isActive() && powerPipe_.isNewData() ) 
            {
                if ( powerPublishTimer.elapsed().toSecondsDouble()>=powerPublishInterval ) {
                    powerPipe_.get( powerData );
                    powerI->localSetAndSend( powerData );
                    powerPublishTimer.restart();
                }
                else {
                    powerI->localSet( powerData );
                }
            }
        }
        catch ( const Ice::ConnectionRefusedException & e )
        {
            context_.tracer()->warning("lost connection to IceStorm");
            // now what?
        }
    } // main loop
    
    }
    catch ( const Ice::CommunicatorDestroyedException & e )
    {
        context_.tracer()->debug( "net handler cought CommunicatorDestroyedException",1);        
        // it's ok, we must be quitting.
    }

    // wait for the component to realize that we are quitting and tell us to stop.
    waitForStop();
    context_.tracer()->debug( "exiting NetHandler thread...",2);
}
