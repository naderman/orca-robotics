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
#include <stdlib.h>
#include <assert.h>
#include <hydroutil/timer.h>

#include <orcaice/orcaice.h>
#include <IceUtil/Thread.h>     // for sleep()

#include "playerclientdriver.h"

using namespace std;
using namespace PlayerCc;

namespace robot2d {

PlayerClientDriver::PlayerClientDriver( const orcaice::Context & context )
    : context_(context)
{
    // parse configuration parameters
    readFromProperties( context, config_ );
    cout<<config_<<endl;
}

PlayerClientDriver::~PlayerClientDriver()
{
}

void
PlayerClientDriver::enable()
{
    robot_.reset();
    positionProxy_.reset();

    stringstream ss;
    ss << "TRACE(playerclientdriver.cpp): PlayerClientDriver: Connecting to player on host "
       << config_.host << ", port " << config_.port << endl;
    context_.tracer()->info( ss.str() );

    // player throws exceptions on creation if we fail
    try
    {
        robot_.reset( new PlayerCc::PlayerClient( config_.host, config_.port ) );
        positionProxy_.reset( new PlayerCc::Position2dProxy( robot_.get(), 0 ) );
    
        robot_->Read();
    
        positionProxy_->SetMotorEnable( true );
    }
    catch ( const PlayerCc::PlayerError & e )
    {
        // Slow things down a little...
        sleep(1);
        stringstream ss;
        ss << "PlayerError("<<e.GetErrorFun()<<"): "<<e.GetErrorStr();
        throw hydroutil::Exception( ERROR_INFO, ss.str() );
    }
}

bool
PlayerClientDriver::read( Data& data )
{
    try {
        // player throws exceptions on creation if we fail
        robot_->Read();
    
        orca::Time t = orcaice::toOrcaTime( positionProxy_->GetDataTime() );
        data.seconds = t.seconds;
        data.useconds = t.useconds;

        data.x = positionProxy_->GetXPos();
        data.y = positionProxy_->GetYPos();
        data.o = positionProxy_->GetYaw();
    
        data.vx = positionProxy_->GetXSpeed();
        data.vy = positionProxy_->GetYSpeed();
        data.w = positionProxy_->GetYawSpeed();
    }
    catch ( PlayerCc::PlayerError &e )
    {
        stringstream ss;
        ss << "PlayerError("<<e.GetErrorFun()<<"): "<<e.GetErrorStr();
        throw hydroutil::Exception( ERROR_INFO, ss.str() );
    }
    return false;
}

void
PlayerClientDriver::write( const Command& command )
{
    try {
        positionProxy_->SetSpeed( command.vx, command.vy, command.w );
    }
    catch ( PlayerCc::PlayerError &e )
    {
        stringstream ss;
        ss << "PlayerError("<<e.GetErrorFun()<<"): "<<e.GetErrorStr();
        throw hydroutil::Exception( ERROR_INFO, ss.str() );
    }
}

void
PlayerClientDriver::getStatus( std::string &status, bool &isWarn, bool &isFault )
{
    status = "";
    isWarn = 0;
    isFault = 0;
}

}
