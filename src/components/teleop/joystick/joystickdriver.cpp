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

#include <linux/input.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>

#include <orcaice/proputils.h>
#include <orcaobj/mathdefs.h>
#include <orcaice/exceptions.h>

#include "joystickdriver.h"
#include "evdevutil.h"

// normalizing macros
#define AXIS_OFFSET         128.0

using namespace std;
using namespace orca;
using namespace teleop;


JoystickDriver::JoystickDriver( const InputDriver::Config &cfg ) :
        config_(cfg)
{
    // init internal data storage
    command_.motion.v.x = 0.0;
    command_.motion.v.y = 0.0;
    command_.motion.w = 0.0;
}

JoystickDriver::~JoystickDriver()
{
}

int 
JoystickDriver::findUSBJoystick( char *joystickDevice )
{
    char device[256];
    int  maxEvdev = 10;
    bool supportsAbsoluteAxes;
    bool isUSB;
    bool foundDevice = false;
    int ret;

    //
    // Search for a joystick
    //
    for ( int i=0; i < maxEvdev; i++ )
    {
        sprintf(device,"/dev/input/event%d",i);

        // printInputType( device );
        // printDeviceInfo( device );

        ret = teleop::devSupportsAbsoluteAxes( device, supportsAbsoluteAxes );

        if ( ret != 0 )
        {
            if ( ret == ENOENT )
            {
                // No such file or directory
                if ( !foundDevice )
                {
                    cout << "ERROR: Couldn't find a USB joystick."<<endl
                         <<"  Searched up to and including /dev/input/event"<<i-1<<"."<<endl;
                }
                else
                {
                    // We can stop looking.
                }
                break;
            }
            else if ( ret == EACCES )
            {
                if ( !foundDevice )
                {
                    cout << "WARNING: Permission to access " << device << " denied." << endl;
                }
                else
                {
                    // We don't really care, since we've already found a useable device.
                }
            }
            continue;
        }

        ret = teleop::devIsUSB( device, isUSB );
        if ( ret != 0 )
        {
            cout << "ERROR: Problem checking for USB status.  This shouldn't happen." << endl;
            exit(1);
        }

        if ( supportsAbsoluteAxes && isUSB )
        {
            if ( foundDevice )
            {
                cout << "ERROR: Found more than one USB device that supports absolute axes.  Don't know which one to use..." << endl;
                cout << "Options are: " << joystickDevice << " and " << device << endl;
            }
            else
            {
                strcpy( joystickDevice, device );
                foundDevice = true;
            }
        }
    }
    if ( !foundDevice )
    {
        return -1;
    }
    else
    {
        return 0;
    }
}

int 
JoystickDriver::enable()
{
    char joystickDevice[256];
    int ret;

    if ( !strcmp(config_.joystickDevice.c_str(),"auto") )
    {
        // Search for something that looks like a USB joystick
        ret = findUSBJoystick( joystickDevice );
    }
    else
    {
        // Use the configured value
        strcpy( joystickDevice, config_.joystickDevice.c_str() );
        jfd_ = open( config_.joystickDevice.c_str(), O_RDONLY );

        if ( jfd_<1 ) 
        {
            cout << "ERROR: could not open joystick on configured device " << config_.joystickDevice << endl;
            return 1;
        }
        ret = 0;
    }

    if ( ret == 0 )
    {
        char name[256] = "Unknown";
        teleop::devName( joystickDevice, name );
        cout << "Reading from joystick \"" << name << "\" on " << joystickDevice << endl;
    }
    else
    {
        cout << "ERROR: Couldn't find anything that looks like a USB Joystick." << endl;
        return -1;
    }

    // Open the fucker for reading input
    jfd_ = open( joystickDevice, O_RDONLY );

    if ( jfd_<1 ) {
        cout << "ERROR: could not open joystick on " << config_.joystickDevice << endl;
        return 1;
    }

    cout<<"enabled."<<endl;
    return 0;
}

int 
JoystickDriver::disable()
{
    close( jfd_ );
    return 0;
}

// read next command from the joystick
int 
JoystickDriver::read( orca::Velocity2dCommand& data )
{
    struct input_event event;

    // get the next event from the joystick
    // note the global scope of this library function
    // (as apposed the member function JoystickDriver::read() )
    if( ::read(jfd_, &event, sizeof(struct input_event) ) < 0 )
    {
        // check if read failed, and if so was it due to disconnection?
        if ( errno == ENODEV ) {
            // set the robot speed and turn rate to zero
            command_.motion.v.x = 0.0;
            command_.motion.v.y = 0.0;
            command_.motion.w = 0.0;

            string errString = "failed to read from joystick";
            throw orcaice::HardwareException( ERROR_INFO, errString );
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

                    command_.motion.v.x = ( AXIS_OFFSET - (double)event.value )
                                                * config_.maxSpeed/AXIS_OFFSET;
    
                    // Square the speed, so it's more sensitive in the centre
                    bool neg = ( command_.motion.v.x < 0 );
                    command_.motion.v.x = ((command_.motion.v.x/config_.maxSpeed)
                            *(command_.motion.v.x/config_.maxSpeed))*config_.maxSpeed;
                    if ( neg ) {
                        command_.motion.v.x *= -1;
                    }
                    //cout << "y:\tvalue = " << event.value << "\tspeed = " << command_.motion.v.x << endl;
                    //cout << "speed = " << command_.motion.v.x << "\tturn rate = " << command_.motion.w << endl;
                    break;
                }
                // SIDE-TO-SIDE
                case ABS_X:
                {
                    // set the robot turn rate
                    command_.motion.w = ( AXIS_OFFSET-(double)event.value )
                                                * config_.maxTurnrate/AXIS_OFFSET;               
    
                    // Square the turnrate, so it's more sensitive in the centre
                    bool neg = ( command_.motion.w < 0 );
                    command_.motion.w = ((command_.motion.w/config_.maxTurnrate)
                            *(command_.motion.w/config_.maxTurnrate))*config_.maxTurnrate;
                    if ( neg ) {
                        command_.motion.w *= -1;
                    }
                    //cout << "x:\tvalue = " << event.value << "\tspeed = " << command_.motion.w << endl;
                    //cout << "speed = " << command_.motion.v.x << "\tturn rate = " << command_.motion.w << endl;               
                    break;
                }
            } // code switch
        } 
    } // type switch

    // apply max limits
    if ( fabs(command_.motion.v.x) > config_.maxSpeed ) {
        command_.motion.v.x =
                (command_.motion.v.x / fabs(command_.motion.v.x)) * config_.maxSpeed;
    }
    if ( fabs(command_.motion.w) > config_.maxTurnrate ) {
        command_.motion.w =
                (command_.motion.w / fabs(command_.motion.w)) * config_.maxTurnrate;
    }

    // return updated command
    // ice_clone() doesn't work for some reason
    //data = command_.ice_clone();
    //data.timeStamp = ?;
    data.motion.v.x = command_.motion.v.x;
    data.motion.v.y = command_.motion.v.y;
    data.motion.w = command_.motion.w;

    return 0;
}
