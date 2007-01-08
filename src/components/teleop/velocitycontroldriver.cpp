/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <orcaice/orcaice.h>
#include "velocitycontroldriver.h"
#include "display.h"
#include "network.h" // for enums

#include <iostream>

using namespace std;
using namespace teleop;

VelocityControl2dDriver::VelocityControl2dDriver( Display* display, const orcaice::Context& context ) :
    display_(display),
    context_(context)
{
    command_.motion.v.x = 0.0;
    command_.motion.v.y = 0.0;
    command_.motion.w = 0.0;

    // one key stroke changes commands by these values
    deltaSpeed_ = 0.05;     // [m/s]
    deltaTurnrate_ = DEG2RAD(2.0);  // [rad/sec]

    maxSpeed_ = 1.0;     // [m/s]
    maxTurnrate_ = DEG2RAD(40.0);  // [rad/sec]
}

VelocityControl2dDriver::~VelocityControl2dDriver() 
{
}
    
int 
VelocityControl2dDriver::enable()
{
    try
    {
        orcaice::connectToInterfaceWithTag<orca::VelocityControl2dPrx>( context_, prx_, "VelocityControl2d" );
    }
    // includes common ex's: ConnectionRefusedException, ConnectTimeoutException
    catch ( const Ice::LocalException & e )
    {
        return 1;
    }

    return 0;
}
    
int 
VelocityControl2dDriver::disable()
{
    return 0;
}

void 
VelocityControl2dDriver::repeatCommand()
{
    try 
    {
        prx_->setCommand( command_ );

        display_->sentRepeatCommand();
    }
    catch ( const Ice::Exception& e )
    {
        command_.motion.v.x = 0.0;
        command_.motion.v.y = 0.0;
        command_.motion.w = 0.0;
        display_->failedToSendCommand();
    }
}

void 
VelocityControl2dDriver::processNewCommandIncrement(int longitudinal, int transverse, int angle )
{
    if ( longitudinal ) {
        command_.motion.v.x += longitudinal*deltaSpeed_;
        command_.motion.v.x = MIN( command_.motion.v.x, maxSpeed_ );
        command_.motion.v.x = MAX( command_.motion.v.x, -maxSpeed_ );
    }

    if ( angle ) {
        command_.motion.w += angle*deltaTurnrate_;
        command_.motion.w = MIN( command_.motion.w, maxTurnrate_ );
        command_.motion.w = MAX( command_.motion.w, -maxTurnrate_ );
    }

    try 
    {
        prx_->setCommand( command_ );

        display_->sentNewVelocityCommand( command_.motion.v.x, command_.motion.v.y, command_.motion.w,
                (fabs(command_.motion.v.x)==maxSpeed_), false, (fabs(command_.motion.w)==maxTurnrate_) );
    }
    catch ( const Ice::Exception& e )
    {
        command_.motion.v.x = 0.0;
        command_.motion.v.y = 0.0;
        command_.motion.w = 0.0;
        display_->failedToSendCommand();
    }
}

void 
VelocityControl2dDriver::processNewRelativeCommand( double longitudinal, double transverse, double angle )
{
    if ( longitudinal != TELEOP_COMMAND_UNCHANGED ) {
        command_.motion.v.x = longitudinal*maxSpeed_;
        command_.motion.v.x = MIN( command_.motion.v.x, maxSpeed_ );
        command_.motion.v.x = MAX( command_.motion.v.x, -maxSpeed_ );
    }

    if ( angle != TELEOP_COMMAND_UNCHANGED ) {
        command_.motion.w = angle*maxTurnrate_;
        command_.motion.w = MIN( command_.motion.w, maxTurnrate_ );
        command_.motion.w = MAX( command_.motion.w, -maxTurnrate_ );
    }

    try 
    {
        prx_->setCommand( command_ );

        display_->sentNewVelocityCommand( command_.motion.v.x, command_.motion.v.y, command_.motion.w,
                (fabs(command_.motion.v.x)==maxSpeed_), false, (fabs(command_.motion.w)==maxTurnrate_) );
    }
    catch ( const Ice::Exception& e )
    {
        command_.motion.v.x = 0.0;
        command_.motion.v.y = 0.0;
        command_.motion.w = 0.0;
        display_->failedToSendCommand();
    }
}
