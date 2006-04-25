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
#ifndef ORCA2_CAMERA_DRIVER_H
#define ORCA2_CAMERA_DRIVER_H

#include <orca/camera.h>
#include <string>


static const ::std::string __orca__cameradriver_default_heartbeat_msg = "";

namespace imageserver {

/*
@brief Abstract class for a camera.

All cameras should implement this, making it simple
to add support for different cameras.

This guy is _not_ guaranteed to be thread-safe!

@author Ben Upcroft b.upcroft at cas.edu.au
*/
class Driver 
{

public:

    Driver() {};
    virtual ~Driver() {};

    virtual int enable()=0;
    virtual int disable()=0;

    virtual bool isEnabled()=0;

    // Blocks till new data is available
    virtual int read( orca::CameraDataPtr &data )=0;

    // TODO: Should also have getData here as we don't always want all info from the read

    // Set a specifc configuration
    virtual int setConfig( const orca::CameraConfigPtr &cfg )=0;

    // Get the current configuration
    virtual int getConfig( orca::CameraConfigPtr &cfg )=0;

    // mechanism to get error messages etc back from driver.
    virtual const std::string &infoMessages() { return infoMessages_; };

    // Any special info to put in the heartbeat messages?
    virtual const std::string heartbeatMessage() { return __orca__cameradriver_default_heartbeat_msg; };

protected:

    std::string infoMessages_;

private:

};

}

#endif
