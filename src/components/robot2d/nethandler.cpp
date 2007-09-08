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

using namespace std;
using namespace robot2d;

void 
NetHandler::convert( const robot2d::Data& internal, orca::Odometry2dData& network )
{
    network.timeStamp.seconds = internal.seconds;
    network.timeStamp.useconds = internal.useconds;

    network.pose.p.x = internal.x;
    network.pose.p.y = internal.y;
    network.pose.o = internal.o;
    
    network.motion.v.x = internal.vx;
    network.motion.v.y = internal.vy;
    network.motion.w = internal.w;
}

void 
NetHandler::convert( const orca::VelocityControl2dData& network, robot2d::Command& internal )
{
    internal.vx = network.motion.v.x;
    internal.w = network.motion.w;
}

NetHandler::NetHandler(
                 orcaice::Proxy<Data>&             dataPipe,
                 orcaice::Notify<Command>&          commandPipe,
                 const orca::VehicleDescription&    descr,
                 const orcaice::Context&            context ) :
    dataPipe_(dataPipe),
    commandPipe_(commandPipe),
    descr_(descr),
    context_(context)
{
}

NetHandler::~NetHandler()
{
}

// This is a direct callback from the VelocityControl2dImpl object.
// It's executed in Ice thread.
// Here we convert to our internal format and stick it into
// another Notify pipe to be handled the HwHandler.
void 
NetHandler::handleData(const orca::VelocityControl2dData& obj)
{
//    cout<<"TRACE(nethandler.cpp): handleData: " << orcaice::toString(obj) << endl;

    robot2d::Command command;
    convert( obj, command );
    commandPipe_.set( command );
}

void
NetHandler::run()
{
    try // this is once per run try/catch: waiting for the communicator to be destroyed
    {
        // multi-try function
        orcaice::activate( context_, this );
    
        std::string prefix = context_.tag() + ".Config.";
    
        // Initialise external interfaces, multi-try init functions
        odometry2dI_ = new orcaifaceimpl::Odometry2dImpl( descr_, "Odometry2d", context_ );
        odometry2dI_->initInterface( this );

        velocityControl2dI_ = new orcaifaceimpl::VelocityControl2dImpl( descr_, "VelocityControl2d", context_ );
        velocityControl2dI_->initInterface( this );
        velocityControl2dI_->setNotifyHandler( this );

        // temp objects in internal format
        Data data;

        // temp objects in network format
        orca::Odometry2dData odometry2dData;

        orcaice::Timer publishTimer;
        double publishInterval = orcaice::getPropertyAsDoubleWithDefault( 
                context_.properties(), prefix+"Odometry2dPublishInterval", 0 );

        const int odometryReadTimeout = 500; // [ms]

        //
        // Main loop
        //
        while( !isStopping() )
        {
//         context_.tracer()->debug( "net handler loop spinning ",1);

            // block on the most frequent data source: odometry
            if ( dataPipe_.getNext( data, odometryReadTimeout ) ) {
//             context_.tracer()->debug( "Net loop timed out", 1);
                continue;
            }

            // Odometry2d
            // convert internal to network format
            convert( data, odometry2dData );
            // check that we were not told to terminate while we were sleeping
            // otherwise, we'll get segfault (there's probably a way to prevent this inside the library)
            if ( !isStopping() && publishTimer.elapsed().toSecondsDouble()>=publishInterval ) {
                odometry2dI_->localSetAndSend( odometry2dData );
                publishTimer.restart();
            } 
            else {
                odometry2dI_->localSet( odometry2dData );
            }
        } // main loop
    
    }
    catch ( const Ice::CommunicatorDestroyedException & e )
    {
        context_.tracer()->debug( "net handler cought CommunicatorDestroyedException",1);        
        // it's ok, we must be quitting.
    }
    catch ( std::exception &e )
    {
        stringstream ss;
        ss << "NetHandler: Caught unexpected exception: " << e.what();
        context_.tracer()->error( ss.str() );
    }
    catch ( ... )
    {
        stringstream ss;
        ss << "NetHandler: Caught unexpected unknown exception.";
        context_.tracer()->error( ss.str() );
    }

    // wait for the component to realize that we are quitting and tell us to stop.
    waitForStop();
    context_.tracer()->debug( "exiting NetHandler thread...",1);
}
