/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>

#include <linux/input.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <string.h>

#include "driver.h"
#include "evdevutil.h"

// normalizing macros
#define AXIS_OFFSET         128.0

using namespace std;

namespace humaninput2djoystick
{

Driver::Driver( const hydroutil::Context &context ) :
    context_(context)
{
}

Driver::~Driver()
{
}

int 
Driver::enable()
{
    std::string prefix = "Joystick.";

    std::string joystickDevice = context_.properties().getPropertyWithDefault( prefix+"Device", "auto" );

    if ( joystickDevice == "auto" ) {
        // Search for something that looks like a USB joystick
        try {
            findUSBJoystick( joystickDevice );
        }
        catch ( const std::string& e ) {
            cout<<e<<endl;
            return 1;
        }
        context_.tracer().info( "Found a USB joystick on device '"+joystickDevice+"'" );
    }

    // Note: we open the joystick in non-blocking mode so read() won't hang on no input
    jfd_ = ::open( joystickDevice.c_str(), O_RDONLY | O_NONBLOCK );

    if ( jfd_<1 )  {
        context_.tracer().error( "Failed to open joystick on device '"+joystickDevice+"'" );
        return 1;
    }

    char name[256] = "Unknown";
    humaninput2djoystick::devName( joystickDevice.c_str(), name );
    context_.tracer().info( "Reading from joystick '"+string(name)+"' on device '"+joystickDevice+"'" );

    context_.tracer().debug( "Joystick driver enabled.",2 );
    return 0;
}

int 
Driver::disable()
{
    close( jfd_ );
    return 0;
}

// read next command from the joystick
int 
Driver::read( Command& command )
{
    struct input_event event;

    // Our strategy here is to poll the joystick for events.  A better
    // method would be to use calls to select(), but this didn't seem
    // to work for reasons unknown.

    // get the next event from the joystick
    // note the global scope of this library function
    // (as apposed the member function Driver::read() )
    int ret = ::read(jfd_, &event, sizeof(struct input_event) );
    if( ret < 0 )
    {
        if ( errno == EAGAIN )
        {
            // No data available
            usleep(10000);
            return TIMEOUT;
        }
        // check if read failed, and if so was it due to disconnection?
        else if ( errno == ENODEV ) {
            // set the robot speed and turn rate to zero
//             network_->newRelativeCommand( 0.0, 0.0, 0.0 );
            command.resetAbsolute();

            return -1;
        }
        cout << "WARNING: joystick read returned error " << strerror(errno) << endl;
    }
    
//     printf( "type 0x%-2x code 0x%-2x value %d\n", event.type, event.code, event.value );
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

//                     network_->newRelativeCommand( speed, TELEOP_COMMAND_UNCHANGED, TELEOP_COMMAND_UNCHANGED );
                    command.resetIncremental();
                    command.isLongIncrement = false;
                    command.longitudinal = speed;

//                     cout << endl<<"DEBUG: joystick y="<<event.value<<" speed="<<speed<<endl;
                    break;
                }
                // SIDE-TO-SIDE
                case ABS_X:
                {     
                    double steer = 1.0-(double)event.value/AXIS_OFFSET;
                    
                    // apply square curve, so it's more sensitive in the centre
                    double sign = steer>0.0 ? 1.0 : -1.0;
                    steer = sign * steer * steer;

//                     network_->newRelativeCommand( TELEOP_COMMAND_UNCHANGED, TELEOP_COMMAND_UNCHANGED, steer );
                    command.resetIncremental();
                    command.isAngularIncrement = false;
                    command.angular = steer;

//                     cout << endl<<"DEBUG: joystick x="<<event.value<<" steer="<<steer<<endl;              
                    break;
                }
            } // code switch
        } 
    } // type switch

    return 0;
}

} // namespace

extern "C" {
    hydrointerfaces::HumanInput2dFactory *createDriverFactory()
    { return new humaninput2djoystick::Factory; }
}
