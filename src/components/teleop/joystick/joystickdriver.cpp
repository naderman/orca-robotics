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

#include <orcaice/orcaice.h>

#include "joystickdriver.h"
#include "evdevutil.h"
#include "../network.h"

// normalizing macros
#define AXIS_OFFSET         128.0

using namespace std;
using namespace teleop;


JoystickDriver::JoystickDriver( Network* network, const orcaice::Context& context ) :
    network_(network),
    context_(context)
{
}

JoystickDriver::~JoystickDriver()
{
}

int 
JoystickDriver::enable()
{
    Ice::PropertiesPtr prop = context_.properties();
    std::string prefix = context_.tag() + ".Config.Joystick.";
    std::string joystickDevice = orcaice::getPropertyWithDefault( prop, prefix+"Device", "auto" );

    if ( joystickDevice == "auto" ) {
        // Search for something that looks like a USB joystick
        try {
            findUSBJoystick( joystickDevice );
        }
        catch ( const std::string& e ) {
            cout<<e<<endl;
            return 1;
        }
        context_.tracer()->info( "Found a USB joystick on device '"+joystickDevice+"'" );
    }

    jfd_ = ::open( joystickDevice.c_str(), O_RDONLY );

    if ( jfd_<1 )  {
        context_.tracer()->error( "Failed to open joystick on device '"+joystickDevice+"'" );
        return 1;
    }

    char name[256] = "Unknown";
    teleop::devName( joystickDevice.c_str(), name );
    context_.tracer()->info( "Reading from joystick '"+string(name)+"' on device '"+joystickDevice+"'" );

    context_.tracer()->debug( "Joystick driver enabled.",2 );
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
JoystickDriver::read()
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
            network_->newRelativeCommand( 0.0, 0.0, 0.0 );

            string errString = "failed to read from joystick";
            throw orcaice::HardwareException( ERROR_INFO, errString );
            return 1;
        }
        else {
            cout << "WARNING: joystick read returned error " << errno << endl;
        }
    }
    
    printf( "======== type 0x%-2x code 0x%-2x value %d\n", event.type, event.code, event.value );
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
                    double speed = 1.0-(double)event.value/AXIS_OFFSET;
                    
                    // apply square curve, so it's more sensitive in the centre
                    double sign = speed>0.0 ? 1.0 : -1.0;
                    speed = sign * speed * speed;

                    network_->newRelativeCommand( speed, TELEOP_COMMAND_UNCHANGED, TELEOP_COMMAND_UNCHANGED );

                    cout << endl<<"DEBUG: joystick y="<<event.value<<" speed="<<speed<<endl;
                    break;
                }
                // SIDE-TO-SIDE
                case ABS_X:
                {     
                    double steer = 1.0-(double)event.value/AXIS_OFFSET;
                    
                    // apply square curve, so it's more sensitive in the centre
                    double sign = steer>0.0 ? 1.0 : -1.0;
                    steer = sign * steer * steer;

                    network_->newRelativeCommand( TELEOP_COMMAND_UNCHANGED, TELEOP_COMMAND_UNCHANGED, steer );

                    cout << endl<<"DEBUG: joystick x="<<event.value<<" steer="<<steer<<endl;              
                    break;
                }
            } // code switch
        } 
    } // type switch

    return 0;
}
