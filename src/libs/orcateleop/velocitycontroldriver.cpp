/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <orcaice/orcaice.h>
#include "velocitycontroldriver.h"
#include "display.h"
#include "network.h"

#include <iostream>

using namespace std;
using namespace orcateleop;

VelocityControl2dDriver::VelocityControl2dDriver( Display* display, const orcaice::Context& context ) :
    display_(display),
    context_(context)
{
    command_.motion.v.x = 0.0;
    command_.motion.v.y = 0.0;
    command_.motion.w = 0.0;

    Ice::PropertiesPtr prop = context_.properties();
    std::string prefix = context_.tag() + ".Config.VelocityControl2d.";

    // one key stroke changes commands by these values
    speedIncrement_ = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"SpeedIncrement", 0.05 );
    turnRateIncrement_ = DEG2RAD( orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"TurnRateIncrement", 2.0 ) );

    maxSpeed_ = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"MaxSpeed", 1.0 );
//     maxSideSpeed_ = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"VelocityControl2d.MaxSideSpeed", 1.0 );
    maxTurnRate_ = DEG2RAD( orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"MaxTurnRate", 40.0 ) );
}

VelocityControl2dDriver::~VelocityControl2dDriver() 
{
}
    
int 
VelocityControl2dDriver::enable()
{
    try
    {
        orcaice::connectToInterfaceWithTag<orca::VelocityControl2dPrx>( context_, prx_, "Generic" );
    }
    // includes common ex's: ConnectionRefusedException, ConnectTimeoutException
    catch ( const Ice::LocalException & e )
    {
        stringstream ss;
        ss << "VelocityControl2dDriver::enable() failed: " << e;
        context_.tracer().warning( ss.str() );
        return 1;
    }

    return 0;
}

    
void 
VelocityControl2dDriver::enableWithString( const string &proxyString )
{
    orcaice::connectToInterfaceWithString<orca::VelocityControl2dPrx>( context_, prx_, proxyString );
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
    catch ( const Ice::CommunicatorDestroyedException& e )
    {
        // we are shutting down, ignore this exception
        command_.motion.v.x = 0.0;
        command_.motion.v.y = 0.0;
        command_.motion.w = 0.0;
    }
    catch ( const orca::OrcaException& e )
    {
        command_.motion.v.x = 0.0;
        command_.motion.v.y = 0.0;
        command_.motion.w = 0.0;
        stringstream ss; ss << "VelocityControl2dDriver::repeatCommand(): " << e.what;
        context_.tracer().warning( ss.str() );
        display_->failedToSendCommand();
    }
    catch ( const Ice::Exception& e )
    {
        command_.motion.v.x = 0.0;
        command_.motion.v.y = 0.0;
        command_.motion.w = 0.0;
        stringstream ss; ss << "VelocityControl2dDriver::repeatCommand(): " << e;
        context_.tracer().warning( ss.str() );
        display_->failedToSendCommand();
    }
}

void 
VelocityControl2dDriver::processMixedCommand( double longitudinal, bool isLongIncrement, 
    double transverse, bool isTransverseIncrement, 
    double angular, bool isAngularIncrement )
{
    if ( isLongIncrement )
        incrementValue( command_.motion.v.x, longitudinal*speedIncrement_, -maxSpeed_, maxSpeed_ );
    else
        setValue( command_.motion.v.x, longitudinal*maxSpeed_, -maxSpeed_, maxSpeed_ );

    if ( isAngularIncrement )
        incrementValue( command_.motion.w, angular*turnRateIncrement_, -maxTurnRate_, maxTurnRate_ );
    else
        setValue( command_.motion.w, angular*maxTurnRate_, -maxTurnRate_, maxTurnRate_ );

    sendCommand();
}

void 
VelocityControl2dDriver::processIncrementCommand(int longitudinal, int transverse, int angle )
{
    incrementValue( command_.motion.v.x, longitudinal*speedIncrement_, -maxSpeed_, maxSpeed_ );
    incrementValue( command_.motion.w, angle*turnRateIncrement_, -maxTurnRate_, maxTurnRate_ );

    sendCommand();
}

void 
VelocityControl2dDriver::processRelativeCommand( double longitudinal, double transverse, double angle )
{
    if ( longitudinal != TELEOP_COMMAND_UNCHANGED ) {
        setValue( command_.motion.v.x, longitudinal*maxSpeed_, -maxSpeed_, maxSpeed_ );
    }

    if ( angle != TELEOP_COMMAND_UNCHANGED ) {
        setValue( command_.motion.w, angle*maxTurnRate_, -maxTurnRate_, maxTurnRate_ );
    }

    sendCommand();
}

void 
VelocityControl2dDriver::sendCommand()
{
    try 
    {
        prx_->setCommand( command_ );

        display_->sentNewVelocityCommand( command_.motion.v.x, command_.motion.v.y, command_.motion.w,
                (fabs(command_.motion.v.x)==maxSpeed_), false, (fabs(command_.motion.w)==maxTurnRate_) );
    }
    catch ( const orca::OrcaException& e )
    {
        command_.motion.v.x = 0.0;
        command_.motion.v.y = 0.0;
        command_.motion.w = 0.0;
        stringstream ss; ss << "VelocityControl2dDriver:processNewCommandIncrement: " << e.what;
        context_.tracer().warning( ss.str() );
        display_->failedToSendCommand();
    }
    catch ( const Ice::Exception& e )
    {
        command_.motion.v.x = 0.0;
        command_.motion.v.y = 0.0;
        command_.motion.w = 0.0;
        stringstream ss; ss << "VelocityControl2dDriver:processNewCommandIncrement: " << e;
        context_.tracer().warning( ss.str() );
        display_->failedToSendCommand();
    }
}
