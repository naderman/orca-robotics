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
#ifndef FAKE_CAMERA_DRIVER_H
#define FAKE_CAMERA_DRIVER_H

#include "driver.h"

#include <iostream>


namespace imageserver {

//
// Bogus camera driver, useful for testing the rest of the component.
//
class FakeDriver : public Driver
{

public:

    FakeDriver();
    virtual ~FakeDriver();

    virtual int enable() { isEnabled_=true; return 0; };
    virtual int disable() { isEnabled_=false; return 0; };

    virtual bool isEnabled() { return isEnabled_; };

    // Blocks till new data is available
    virtual int read( orca::CameraDataPtr &data );

    // Set a specific configuration
    virtual int setConfig( const orca::CameraConfigPtr &cfg );

    // Get the current configuration
    virtual int getConfig( orca::CameraConfigPtr &cfg );

    // fill an image with values
    int fill( orca::ByteSequence& image, const unsigned char R, const unsigned char G, const unsigned B );

private:

    bool isEnabled_;

};

}

#endif
