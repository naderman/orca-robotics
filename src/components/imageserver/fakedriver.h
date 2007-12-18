/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef FAKE_CAMERA_DRIVER_H
#define FAKE_CAMERA_DRIVER_H

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
    virtual int read( orca::CameraData& data );

private:

    orcaice::Context context_;

    // fill an image with values
    int flatColor( orca::ByteSequence& image, unsigned char R, unsigned char G, unsigned char B );
    int randomColor( orca::ByteSequence& image );
    int fromFile( orca::ByteSequence& image, const std::string & filename );

};

}

#endif
