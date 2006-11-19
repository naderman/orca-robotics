/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef ORCA2_IMAGESERVER_FAKE_CAMERA_DRIVER_H
#define ORCA2_IMAGESERVER_FAKE_CAMERA_DRIVER_H

#include <orcaice/context.h>
#include "driver.h"

namespace imageserver {

//
// Bogus camera driver, useful for testing the rest of the component.
//
class FakeDriver : public Driver
{

public:

    FakeDriver( const Config & cfg, const orcaice::Context & context );
    virtual ~FakeDriver();

    virtual int init();

    // Blocks till new data is available
    virtual int read( orca::CameraDataPtr &data );

private:

    orcaice::Context context_;

    // fill an image with values
    int fill( orca::ByteSequence& image, const unsigned char R, const unsigned char G, const unsigned B );

};

}

#endif
