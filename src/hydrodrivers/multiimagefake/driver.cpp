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
#include <sstream>
#include <cmath>
#include <IceUtil/IceUtil.h>
#include <gbxutilacfr/gbxutilacfr.h>

#include "driver.h"

using namespace std;

namespace multiimagefake {

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
Driver::read( hydrointerfaces::MultiImage::Data &data )
{
    context_.tracer().debug( "Generating fake image data..." );

    switch ( mode_ )
    {
    case Noise :
        for ( Data::size_type i=0; i < data.size(); ++i )
        {
            for( uint32_t j=0; j < config_[i].size; ++j)
            {
//                 std::stringstream ss;
//                 ss << "generating noise for image " << i << " has size " << config_[i].size << std::endl;
//                 context_.tracer().debug( ss.str(), 10 );
                data[i].pixelData[j] = rand() % 256;
            }
            IceUtil::Time t = IceUtil::Time::now();
            data[i].timeStampSec = (int)t.toSeconds();
            data[i].timeStampUsec = (int)t.toMicroSeconds() - data[i].timeStampSec*1000000;
        }
        break;
    case Smooth :
        // alexm: the color changes can be made much cooler.
        prevIntensity_++;
        for ( Data::size_type i=0; i < config_.size(); ++i )
        {
            for( uint32_t j=0; j < config_[i].size; ++j )
            {
                data[i].pixelData[j] = prevIntensity_;
            }
            IceUtil::Time t = IceUtil::Time::now();
            data[i].timeStampSec = (int)t.toSeconds();
            data[i].timeStampUsec = (int)t.toMicroSeconds() - data[i].timeStampSec*1000000;
        }
        break;
    }

    IceUtil::ThreadControl::sleep(IceUtil::Time::milliSeconds(20));
}

} // namespace

extern "C" {
    hydrointerfaces::MultiImageFactory *createMultiImageDriverFactory()
    { return new multiimagefake::Factory; }
}
