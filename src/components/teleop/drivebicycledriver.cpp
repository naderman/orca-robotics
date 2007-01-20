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
#include "drivebicycledriver.h"
#include "display.h"
#include "network.h"

#include <iostream>

using namespace std;
using namespace teleop;

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
    catch ( const Ice::Exception& e )
    {
        command_.speed = 0.0;
        command_.steerAngle = 0.0;
        display_->failedToSendCommand();
    }
}

void 
DriveBicycleDriver::processNewCommandIncrement(int longitudinal, int transverse, int angle )
{
cout<<"DEBUG: DriveBicycleDriver::processNewCommandIncrement"<<endl;
    if ( longitudinal ) {
        command_.speed += longitudinal*speedIncrement_;
        command_.speed = MIN( command_.speed, maxSpeed_ );
        command_.speed = MAX( command_.speed, -maxSpeed_ );
    }

    if ( angle ) {
        command_.steerAngle += angle*steerAngleIncrement_;
        command_.steerAngle = MIN( command_.steerAngle, maxSteerAngle_ );
        command_.steerAngle = MAX( command_.steerAngle, -maxSteerAngle_ );
    }

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
        display_->failedToSendCommand();
    }
}

void 
DriveBicycleDriver::processNewRelativeCommand( double longitudinal, double transverse, double angle )
{
cout<<"DEBUG: DriveBicycleDriver::processNewRelativeCommand"<<endl;
    if ( longitudinal != TELEOP_COMMAND_UNCHANGED ) {
        command_.speed = longitudinal*maxSpeed_;
        command_.speed = MIN( command_.speed, maxSpeed_ );
        command_.speed = MAX( command_.speed, -maxSpeed_ );
    }

    if ( angle != TELEOP_COMMAND_UNCHANGED ) {
        command_.steerAngle = angle*maxSteerAngle_;
        command_.steerAngle = MIN( command_.steerAngle, maxSteerAngle_ );
        command_.steerAngle = MAX( command_.steerAngle, -maxSteerAngle_ );
    }

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
        display_->failedToSendCommand();
    }
}
