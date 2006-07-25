/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
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
