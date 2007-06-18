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
#include "velocitycontrol2dI.h"

using namespace std;
using namespace segwayrmp;

namespace {
    const char *SUBSYSTEM_NAME = "network";
}

void 
NetHandler::convert( const Data& internal, orca::Odometry2dData& network )
{
    network.timeStamp.seconds = internal.seconds;
    network.timeStamp.useconds = internal.useconds;

    network.pose.p.x = internal.x;
    network.pose.p.y = internal.y;
    network.pose.o = internal.yaw;
    
    network.motion.v.x = internal.vx;
    network.motion.v.y = 0.0;
    network.motion.w = internal.dyaw;
}

void 
NetHandler::convert( const Data& internal, orca::Odometry3dData& network )
{
    network.timeStamp.seconds = internal.seconds;
    network.timeStamp.useconds = internal.useconds;

    network.pose.p.x = internal.x;
    network.pose.p.y = internal.y;
    network.pose.p.z = 0.0;

    network.pose.o.r = internal.roll;
    network.pose.o.p = internal.pitch;
    network.pose.o.y = internal.yaw;
    
    network.motion.v.x = internal.vx;
    network.motion.v.y = 0.0;
    network.motion.v.z = 0.0;

    network.motion.w.x = internal.droll;
    network.motion.w.y = internal.dpitch;
    network.motion.w.z = internal.dyaw;
}

void 
NetHandler::convert( const Data& internal, orca::PowerData& network )
{
    network.timeStamp.seconds = internal.seconds;
    network.timeStamp.useconds = internal.useconds;

    network.batteries[0].voltage = internal.mainvolt;
    network.batteries[1].voltage = internal.mainvolt;
    network.batteries[2].voltage = internal.uivolt;
    
    network.batteries[0].isBatteryCharging = orca::ChargingUnknown;
    network.batteries[1].isBatteryCharging = orca::ChargingUnknown;
    network.batteries[2].isBatteryCharging = orca::ChargingUnknown;
}

void 
NetHandler::convert( const orca::VelocityControl2dData& network, segwayrmp::Command& internal )
{
    internal.vx = network.motion.v.x;
    internal.w = network.motion.w;
}

NetHandler::NetHandler(
        orcaice::Proxy<Data>&               dataPipe,
        orcaice::Notify<orca::VelocityControl2dData>&  commandPipe,
        const orca::VehicleDescription&               descr,
        const orcaice::Context&                    context ) :
    dataPipe_(dataPipe),
    commandPipe_(commandPipe),
    descr_(descr),
    context_(context)
{
    context_.status()->setMaxHeartbeatInterval( SUBSYSTEM_NAME, 10.0 );
    context_.status()->initialising( SUBSYSTEM_NAME );
}

NetHandler::~NetHandler()
{
}

void
NetHandler::activate()
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
        context_.status()->initialising( SUBSYSTEM_NAME, "activating..." );
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(2));
    }
}

void
NetHandler::initOdom2d()
{
    //
    // PROVIDED: Odometry2d
    //
    odometry2dI_ =
        new orcaifaceimpl::Odometry2dIface( descr_, "Odometry2d", context_ );

    while ( isActive() ) {
        try {
            odometry2dI_->initInterface();
            context_.tracer()->debug( "odometry 2d interface initialized",2);
            break;
        }
        catch ( const orcaice::NetworkException& e ) {
            stringstream ss;
            ss << "Failed to setup interface: " << e.what() << ".  Check Registry and IceStorm. Will try again in 2 secs...";
            context_.tracer()->warning( ss.str() );
        }
        catch ( const Ice::Exception& e ) {
            stringstream ss;
            ss << "Failed to setup interface: " << e << ".  Check Registry and IceStorm. Will try again in 2 secs...";
            context_.tracer()->warning( ss.str() );
        }

        context_.status()->initialising( SUBSYSTEM_NAME, "initOdom2d..." );
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(2));
    }
}

void
NetHandler::initOdom3d()
{
    //
    // PROVIDED: Odometry3d
    //
    odometry3dI_ = 
        new orcaifaceimpl::Odometry3dIface( descr_, "Odometry3d", context_ );


    while ( isActive() ) {
        try {
            odometry3dI_->initInterface();
            context_.tracer()->debug( "odometry 3d interface initialized",2);
            break;
        }
        catch ( const orcaice::NetworkException& e ) {
            stringstream ss;
            ss << "Failed to setup interface: " << e.what() << ".  Check Registry and IceStorm. Will try again in 2 secs...";
            context_.tracer()->warning( ss.str() );
        }
        catch ( const Ice::Exception& e ) {
            stringstream ss;
            ss << "Failed to setup interface: " << e << ".  Check Registry and IceStorm. Will try again in 2 secs...";
            context_.tracer()->warning( ss.str() );
        }
        
        context_.status()->initialising( SUBSYSTEM_NAME, "initOdom3d..." );
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(2));
    }    
}

void
NetHandler::initPower()
{
    //
    // PROVIDED: Power
    //
    powerI_ = new orcaifaceimpl::PowerIface( "Power", context_ );


    while ( isActive() ) {
        try {
            powerI_->initInterface();
            context_.tracer()->debug( "power interface initialized",2);
            break;
        }
        catch ( const orcaice::NetworkException& e ) {
            stringstream ss;
            ss << "Failed to setup interface: " << e.what() << ".  Check Registry and IceStorm. Will try again in 2 secs...";
            context_.tracer()->warning( ss.str() );
        }
        catch ( const Ice::Exception& e ) {
            stringstream ss;
            ss << "Failed to setup interface: " << e << ".  Check Registry and IceStorm. Will try again in 2 secs...";
            context_.tracer()->warning( ss.str() );
        }
        
        context_.status()->initialising( SUBSYSTEM_NAME, "initPower..." );
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(2));
    }
}


void
NetHandler::run()
{
    try // this is once per run try/catch: waiting for the communicator to be destroyed
    {
        activate();
    
        std::string prefix = context_.tag() + ".Config.";

        //
        // PROVIDED: VelocityControl2d
        //

        // create servant for direct connections and tell adapter about it
        // don't need to store it as a member variable, adapter will keep it alive
        Ice::ObjectPtr velocityControl2dI = new VelocityControl2dI( descr_, commandPipe_ );

        // two possible exceptions will kill it here, that's what we want
        orcaice::createInterfaceWithTag( context_, velocityControl2dI, "VelocityControl2d" );

        // Initialise external interfaces
        // alexm: this option used to be in HwHandler. The external interface was always enabled,
        //        but when ProvideOdometry3d=0, the HwHandler would not push any 3D data 
        //        to NetHandler.
//         bool provideOdometry3d = (bool)orcaice::getPropertyAsIntWithDefault( 
//                 context_.properties(), prefix+".ProvideOdometry3d", 1 );
        initOdom2d();
        initOdom3d();
        initPower();

        // temp objects in internal format
        Data data;

        // temp objects in network format
        orca::Odometry2dData odometry2dData;
        orca::Odometry3dData odometry3dData;
        orca::PowerData powerData;

        // set up data structure for 3 batteries
        powerData.batteries.resize(3);
        powerData.batteries[0].name = "main-front";
        powerData.batteries[1].name = "main-rear";
        powerData.batteries[2].name = "ui";

        orcaice::Timer odometry2dPublishTimer;
        orcaice::Timer odometry3dPublishTimer;
        orcaice::Timer powerPublishTimer;

        double odometry2dPublishInterval = orcaice::getPropertyAsDoubleWithDefault( 
                context_.properties(), prefix+"Odometry2dPublishInterval", 0.1 );
        double odometry3dPublishInterval = orcaice::getPropertyAsDoubleWithDefault( 
                context_.properties(), prefix+"Odometry3dPublishInterval", 0.1 );
        double powerPublishInterval = orcaice::getPropertyAsDoubleWithDefault( 
                context_.properties(), prefix+"PowerPublishInterval", 20.0 );

        const int odometryReadTimeout = 500; // [ms]
        context_.status()->setMaxHeartbeatInterval( "network", 2.0*(odometryReadTimeout/1000.0) );

        //
        // Main loop
        //
        while( isActive() )
        {
//         context_.tracer()->debug( "net handler loop spinning ",1);

            // block on the only incoming data stream
            if ( dataPipe_.getNext( data, odometryReadTimeout ) ) {
//             context_.tracer()->debug( "Net loop timed out", 1);
                // Don't flag this as an error -- it may happen during normal initialisation.
                context_.status()->ok( SUBSYSTEM_NAME, "Net loop timed out" );
                continue;
            }

            // Odometry2d
            convert( data, odometry2dData );
            // check that we were not told to terminate while we were sleeping
            // otherwise, we'll get segfault (there's probably a way to prevent this inside the library)
            if ( isActive() && odometry2dPublishTimer.elapsed().toSecondsDouble()>=odometry2dPublishInterval ) {
                odometry2dI_->localSetAndSend( odometry2dData );
                odometry2dPublishTimer.restart();
            } 
            else {
                odometry2dI_->localSet( odometry2dData );
            }

            // Odometry3d
            convert( data, odometry3dData );
            if ( isActive() && odometry3dPublishTimer.elapsed().toSecondsDouble()>=odometry3dPublishInterval ) {
                odometry3dI_->localSetAndSend( odometry3dData );
                odometry3dPublishTimer.restart();
            } 
            else {
                odometry3dI_->localSet( odometry3dData );
            }

            // Power
            convert( data, powerData );
            if ( isActive() && powerPublishTimer.elapsed().toSecondsDouble()>=powerPublishInterval ) {
                powerI_->localSetAndSend( powerData );
                powerPublishTimer.restart();
            } 
            else {
                powerI_->localSet( powerData );
            }

            // subsystem heartbeat
            context_.status()->ok( SUBSYSTEM_NAME );
        } // main loop
    
    }
    catch ( const Ice::CommunicatorDestroyedException & e )
    {
        context_.tracer()->debug( "NetHandler cought CommunicatorDestroyedException",1);        
        // it's ok, we must be quitting.
    }
    catch ( std::exception &e )
    {
        stringstream ss;
        ss << "NetHandler caught unexpected exception: " << e.what();
        context_.tracer()->error( ss.str() );
        context_.status()->fault( SUBSYSTEM_NAME, ss.str() );
    }
    catch ( ... )
    {
        stringstream ss;
        ss << "NetHandler caught unknown unexpected exception";
        context_.tracer()->error( ss.str() );
        context_.status()->fault( SUBSYSTEM_NAME, ss.str() );
    }

    // wait for the component to realize that we are quitting and tell us to stop.
    waitForStop();
    context_.tracer()->debug( "Exiting NetHandler thread...",2);
}
