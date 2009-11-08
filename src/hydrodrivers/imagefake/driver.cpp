/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <stdint.h>
#include <iostream>
#include <cmath>
#include <IceUtil/IceUtil.h>
#include <gbxutilacfr/gbxutilacfr.h>

#include "driver.h"

using namespace std;

namespace imagefake {

Driver::Driver( const Config &cfg, const hydroutil::Context &context ) :
    prevIntensity_(0),
    config_(cfg),
    context_(context)
{
    hydroutil::Properties &prop = context_.properties();
    std::string prefix = "Fake.";
    string mode = prop.getPropertyWithDefault( prefix+"Mode", "noise" );
    if ( mode == "noise" )
        mode_ = Noise;
    else if ( mode == "smooth" )
        mode_ = Smooth;
    else
        throw gbxutilacfr::Exception( ERROR_INFO, "Unknown image driver mode: '"+mode+"'" );
}

Driver::~Driver()
{
}

void
Driver::read( hydrointerfaces::Image::Data &data )
{
    context_.tracer().debug( "Generating fake image data..." );

    switch ( mode_ )
    {
    case Noise :
        for ( uint32_t i=0; i < config_.size; i++ )
        {
            data.pixelData[i] = rand() % 256;
        }
        break;
    case Smooth :
        // alexm: the color changes can be made much cooler.
        prevIntensity_++;
        for ( uint32_t i=0; i < config_.size; i++ )
        {
            data.pixelData[i] = prevIntensity_;
        }
        break;
    }

    IceUtil::Time t = IceUtil::Time::now();
    data.timeStampSec = (int)t.toSeconds();
    data.timeStampUsec = (int)t.toMicroSeconds() - data.timeStampSec*1000000;
    IceUtil::ThreadControl::sleep(IceUtil::Time::milliSeconds(20));
}

} // namespace

extern "C" {
    hydrointerfaces::ImageFactory *createImageDriverFactory()
    { return new imagefake::Factory; }
}
