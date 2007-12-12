/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2007 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>

// orca includes
#include <orcaice/orcaice.h>
#include <orca/odometry2d.h>
#include "handler.h"

// yarp includes
#include <yarp/os/Network.h>
#include <yarp/os/Port.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Time.h>

using namespace std;

namespace orca2yarp {

Handler::Handler( const orcaice::Context& context ) :
    context_(context)
{
}

Handler::~Handler()
{
}

void
Handler::run()
{
    // we are in a different thread now, catch all stray exceptions
    try
    {

    // read configuration parameters (config file + command line)
    std::string prefix = context_.tag()+".Config.";
    std::string yarpPortName = orcaice::getPropertyWithDefault( 
            context_.properties(), prefix+"Yarp.PortName", "/sender" );

    // activate component, this may throw an exception
    context_.activate();

    // connect to required interface Odometry2d
    // this is a remote call which can fail in many ways, catch exceptions
    orca::Odometry2dPrx odometry2dPrx;
    orcaice::connectToInterfaceWithTag<orca::Odometry2dPrx>( 
            context_, odometry2dPrx, "Odometry2d" );

    // orca data structure
    orca::Odometry2dData odometry2dData;

    // init Yarp
    yarp::os::Network::init();
    yarp::os::Port output;
    output.open( yarpPortName.c_str() );

    //
    // Main loop, ctrl-C will exit from this
    //   
    while( !isStopping() )
    {
        // Instead of pulling data every time with getData(), 
        // should really subscribe and wait the server to push data to us.
        // This is a remote call, must catch exceptions here!
        odometry2dData = odometry2dPrx->getData();
        context_.tracer().print( orcaice::toString( odometry2dData ) );

        // prepare a yarp message
        yarp::os::Bottle bot; 
        bot.addString("sec=");
        bot.addInt( odometry2dData.timeStamp.seconds );
        bot.addString(" usec=");
        bot.addInt( odometry2dData.timeStamp.useconds );
        bot.addString(" x=");
        bot.addDouble( odometry2dData.pose.p.x );
        bot.addString(" y=");
        bot.addDouble( odometry2dData.pose.p.y );
        bot.addString(" w=");
        bot.addDouble( odometry2dData.pose.o );

        // send the yarp message
        context_.tracer().debug( "Sending yarp message", 5 );
        output.write(bot);

        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(1));
    }

    //
    // unexpected exceptions
    //
    } // try
    catch ( ... )
    {
        context_.tracer().error( "Unexpected exception from somewhere.");
        context_.communicator()->destroy();
    }

    // wait for the component to realize that we are quitting and tell us to stop.
    waitForStop();
    context_.tracer().debug( "Handler stopped", 2 );
}

} // namespace
