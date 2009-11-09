/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <cmath>
#include <iostream>
#include <limits>
#include <sstream>

#include <IceUtil/IceUtil.h>

#include <gbxutilacfr/gbxutilacfr.h>

#include "driver.h"

using namespace std;

namespace disparitysimple {

Driver::Driver( const Config &config 
              , const hydroutil::Context &context ) 
    : config_(config)
    , context_(context)
{
    if(config_.format != "BGR8" && config_.format != "RGB8")
    {
        std::stringstream ss;
        ss << "This algorithm only accepts BGR8 or RGB8 images, got \'"  << config_.format << "\'";
        throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
    }

    if(config_.shifts > std::numeric_limits<unsigned char>::max() )
    {
        std::stringstream ss;
        ss << "This algorithm only accepts shift values that fit in to an un unsigned char, max value is " << std::numeric_limits<unsigned char>::max() << " and given shift value was " << config_.shifts;
        throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
    }
}

Driver::~Driver()
{
}

void
Driver::process( const Data &leftImageData, const Data &rightImageData, Data &outputImageData  )
{
    context_.tracer().debug( "Generating disparity image data..." );

    unsigned char * leftimg = leftImageData.pixelData;
    unsigned char * rightimg = rightImageData.pixelData;
    unsigned char * outimg = outputImageData.pixelData;
    
    // compute the intensity difference for each pixel at config_.shifts shifts of the right image
    // and only keep track of the smallest one. That shift value then gets stored in to the outputImage
    size_t k = 0;
    for( size_t y = 0; y < config_.height; ++y )
    {
        for( size_t x = 0; x < config_.width; ++x )
        {
            unsigned char minval = std::numeric_limits<unsigned char>::max();
            unsigned char disparity = 0;
            size_t m = 0;
            int rightval = (rightimg[k] + rightimg[k+1] + rightimg[k+2])/3;

            for(unsigned char j = 0; (j < config_.shifts) && (x+j < config_.width); ++j)
            {
                int leftval = (leftimg[k+m] + leftimg[k+m+1] + leftimg[k+m+2])/3;
                unsigned int diff = abs(leftval - rightval);
                if( diff < minval )
                {
                    minval = diff;
                    disparity = j;
                }
                m += 3;
            }
            k += 3;

            std::stringstream ss;
            ss << "disparity at " << x << " , " << y << " : " << static_cast<unsigned int>(disparity);

            context_.tracer().debug( ss.str(), 9 );

            outimg[y*config_.width+x] = disparity*(255/config_.shifts); //stretch values so its easier to see
        }
    }
}

} // namespace

extern "C" {
    hydrointerfaces::DisparityFactory *createDisparityDriverFactory()
    { return new disparitysimple::Factory; }
}
