/*
 *  Orca Project: Components for robotics.
 *
 *  Copyright (C) 2004-2006
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <iostream>

#include <linux/input.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include "teleopjoystickdriver.h"

#include <orcaiceutil/proputils.h>
#include <orcaiceutil/mathdefs.h>
#include <orcaiceutil/exceptions.h>

// normalizing macros
#define AXIS_OFFSET         128.0

using namespace std;
using namespace orca;


TeleopJoystickDriver::TeleopJoystickDriver( const InputDriver::Config &cfg ) :
        config_(cfg)
{
    // init internal data storage
    command_ = new Velocity2dCommand;
    command_->motion.v.x = 0.0;
    command_->motion.v.y = 0.0;
    command_->motion.w = 0.0;
}

TeleopJoystickDriver::~TeleopJoystickDriver()
{
}

int TeleopJoystickDriver::enable()
{
    // joystick port
    std::string joystickPort = "/dev/input/event0";
    //xmlDoc->getParam( "joystickPort", joystickPort, DEFAULT_JOYSTICK_PORT );

    jfd_ = open( joystickPort.c_str(), O_RDONLY );

    if ( jfd_<1 ) {
        cout << "ERROR: could not open joystick on " << joystickPort << endl;
        return 1;
    }

    char name[256] = "Unknown";

    if ( ioctl( jfd_, EVIOCGNAME(sizeof(name)), name ) < 0 ) {
        cerr<<"error reading joystick name" << endl;
        return 1;
    }
    cout<<"reading from joystick \"" << name << "\" on " << joystickPort << endl;

    cout<<"enabled."<<endl;
    return 0;
}

int TeleopJoystickDriver::disable()
{
    close( jfd_ );
    return 0;
}

// read next command from the joystick
int TeleopJoystickDriver::readdata( orca::Velocity2dCommandPtr &data )
{
    struct input_event event;

    // get the next event from the joystick
    if( read(jfd_, &event, sizeof(struct input_event) ) < 0 )
    {
        // check if read failed, and if so was it due to disconnection?
        if ( errno == ENODEV ) {
            // set the robot speed and turn rate to zero
            command_->motion.v.x = 0.0;
            command_->motion.v.y = 0.0;
            command_->motion.w = 0.0;

            string errString = "failed to read from joystick";
            throw orcaiceutil::OrcaIceUtilHardwareException( ERROR_INFO, errString );
            return 1;
        }
        else {
            cout << "WARNING: joystick read returned error " << errno << endl;
        }
    }
    
    //printf( "type 0x%-2x code 0x%-2x value %d\n", event.type, event.code, event.value );
    switch( event.type )
    {
        // we respond to only one event type
        case EV_ABS:
        {
            switch( event.code )
            {
                // FORWARD-AND-BACK
                case ABS_Y:  
                {
                    // set the robot velocity

                    command_->motion.v.x = ( AXIS_OFFSET - (double)event.value )
                                                * config_.maxSpeed/AXIS_OFFSET;
    
                    // Square the speed, so it's more sensitive in the centre
                    bool neg = ( command_->motion.v.x < 0 );
                    command_->motion.v.x = ((command_->motion.v.x/config_.maxSpeed)
                            *(command_->motion.v.x/config_.maxSpeed))*config_.maxSpeed;
                    if ( neg ) {
                        command_->motion.v.x *= -1;
                    }
                    //cout << "y:\tvalue = " << event.value << "\tspeed = " << command_->motion.v.x << endl;
                    //cout << "speed = " << command_->motion.v.x << "\tturn rate = " << command_->motion.w << endl;
                    break;
                }
                // SIDE-TO-SIDE
                case ABS_X:
                {
                    // set the robot turn rate
                    command_->motion.w = ( AXIS_OFFSET-(double)event.value )
                                                * config_.maxTurnrate/AXIS_OFFSET;               
    
                    // Square the turnrate, so it's more sensitive in the centre
                    bool neg = ( command_->motion.w < 0 );
                    command_->motion.w = ((command_->motion.w/config_.maxTurnrate)
                            *(command_->motion.w/config_.maxTurnrate))*config_.maxTurnrate;
                    if ( neg ) {
                        command_->motion.w *= -1;
                    }
                    //cout << "x:\tvalue = " << event.value << "\tspeed = " << command_->motion.w << endl;
                    //cout << "speed = " << command_->motion.v.x << "\tturn rate = " << command_->motion.w << endl;               
                    break;
                }
            } // code switch
        } 
    } // type switch

    // apply max limits
    if ( fabs(command_->motion.v.x) > config_.maxSpeed ) {
        command_->motion.v.x =
                (command_->motion.v.x / fabs(command_->motion.v.x)) * config_.maxSpeed;
    }
    if ( fabs(command_->motion.w) > config_.maxTurnrate ) {
        command_->motion.w =
                (command_->motion.w / fabs(command_->motion.w)) * config_.maxTurnrate;
    }

    // return updated command
    // ice_clone() doesn't work for some reason
    //data = command_->ice_clone();
    //data->timeStamp = ?;
    data->motion.v.x = command_->motion.v.x;
    data->motion.v.y = command_->motion.v.y;
    data->motion.w = command_->motion.w;

    return 0;
}
