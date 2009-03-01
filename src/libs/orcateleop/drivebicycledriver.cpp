/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <orcaice/orcaice.h>
#include "drivebicycledriver.h"
#include "display.h"
#include "network.h"

#include <iostream>

using namespace std;
using namespace orcateleop;

DriveBicycleDriver::DriveBicycleDriver( Display* display, const orcaice::Context& context ) :
    display_(display),
    context_(context)
{
    command_.speed = 0.0;
    command_.steerAngle = 0.0;

    Ice::PropertiesPtr prop = context_.properties();
    std::string prefix = context_.tag() + ".Config.DriveBicycle.";

    // one key stroke changes commands by these values
    speedIncrement_ = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"SpeedIncrement", 0.05 );
    steerAngleIncrement_ = DEG2RAD( orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"SteerAngleIncrement", 5.0 ) );

    maxSpeed_ = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"MaxSpeed", 1.0 );
    maxSteerAngle_ = DEG2RAD( orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"MaxSteerAngle", 40.0 ) );
}

DriveBicycleDriver::~DriveBicycleDriver() 
{
}
    
int 
DriveBicycleDriver::enable()
{
    try
    {
        orcaice::connectToInterfaceWithTag<orca::DriveBicyclePrx>( context_, prx_, "Generic" );
    }
    // includes common ex's: ConnectionRefusedException, ConnectTimeoutException
    catch ( const Ice::LocalException & e )
    {
        return 1;
    }

    return 0;
}
    
int 
DriveBicycleDriver::disable()
{
    return 0;
}

void 
DriveBicycleDriver::repeatCommand()
{
    try 
    {
        prx_->setCommand( command_ );

        display_->sentRepeatCommand();
    }
    catch ( const Ice::CommunicatorDestroyedException& e )
    {
        // we are shutting down, ignore this exception
        command_.speed = 0.0;
        command_.steerAngle = 0.0;
    }
    catch ( const Ice::Exception& e )
    {
        command_.speed = 0.0;
        command_.steerAngle = 0.0;
        stringstream ss; ss << "DriveBicycleDriver::repeatCommand(): " << e;
        context_.tracer().warning( ss.str() );
        display_->failedToSendCommand();
    }
}

void 
DriveBicycleDriver::processMixedCommand( double longitudinal, bool isLongIncrement, 
    double transverse, bool isTransverseIncrement, 
    double angular, bool isAngularIncrement )
{
    if ( isLongIncrement )
        incrementValue( command_.speed, longitudinal*speedIncrement_, -maxSpeed_, maxSpeed_ );
    else
        setValue( command_.speed, longitudinal*maxSpeed_, -maxSpeed_, maxSpeed_ );

    if ( isAngularIncrement )
        incrementValue( command_.steerAngle, angular*steerAngleIncrement_, -maxSteerAngle_, maxSteerAngle_ );
    else
        setValue( command_.steerAngle, angular*maxSteerAngle_, -maxSteerAngle_, maxSteerAngle_ );

    sendCommand();
}

void 
DriveBicycleDriver::processIncrementCommand(int longitudinal, int transverse, int angular )
{
// cout<<"DEBUG: DriveBicycleDriver::processNewCommandIncrement"<<endl;
    incrementValue( command_.speed, longitudinal*speedIncrement_, -maxSpeed_, maxSpeed_ );
    incrementValue( command_.steerAngle, angular*steerAngleIncrement_, -maxSteerAngle_, maxSteerAngle_ );

    sendCommand();
}

void 
DriveBicycleDriver::processRelativeCommand( double longitudinal, double transverse, double angular )
{
// cout<<"DEBUG: DriveBicycleDriver::processNewRelativeCommand"<<endl;
    if ( longitudinal != TELEOP_COMMAND_UNCHANGED ) {
        setValue( command_.speed, longitudinal*maxSpeed_, -maxSpeed_, maxSpeed_ );
    }

    if ( angular != TELEOP_COMMAND_UNCHANGED ) {
        setValue( command_.steerAngle, angular*maxSteerAngle_, -maxSteerAngle_, maxSteerAngle_ );
    }

// cout<<"DEBUG: DriveBicycleDriver::processNewRelativeCommand: sending command : speed="<<command_.speed<<" steer="<<command_.steerAngle<<endl;
    sendCommand();
}

void
DriveBicycleDriver::sendCommand()
{
    try 
    {
        prx_->setCommand( command_ );

        display_->sentNewBicycleCommand( command_.speed, command_.steerAngle,
                (fabs(command_.speed)==maxSpeed_), (fabs(command_.steerAngle)==maxSteerAngle_) );
    }
    catch ( const Ice::Exception& e )
    {
        command_.speed = 0.0;
        command_.steerAngle = 0.0;
        stringstream ss; ss << "DriveBicycleDriver: " << e;
        context_.tracer().warning( ss.str() );
        display_->failedToSendCommand();
    }
}
