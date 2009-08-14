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
        ss << "This algorithm only excepts BGR8 or RGB8 images, got \'"  << config_.format << "\'";
        throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
    }

    numOfShifts_ = 32;
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
    
    // compute the intensity difference for each pixel at numOfShifts_ shifts of the right image
    // and only keep track of the smallest one. That value then gets stored in to the outputImage
    size_t k = 0;
    for( size_t i = 0; i < config_.width*config_.height; ++i)
    {
        unsigned char minval = std::numeric_limits<unsigned char>::max();
        unsigned char disparity = 0;
        size_t m = 0;
        int rightval = (rightimg[k] + rightimg[k+1] + rightimg[k+2])/3;
        for(unsigned int j = 0; j < numOfShifts_; ++j)
        {
            int leftval = (leftimg[k+m] + leftimg[k+m+1] + leftimg[k+m+2])/3;
            unsigned int diff = abs(leftval - rightval);
            if( diff < minval )
            {
                minval = diff;
                disparity = j*numOfShifts_; //stretch values so its easier to see
            }
            m += 3;
        }
        k += 3;

        std::stringstream ss;
        ss << "disparity at " << i % config_.width << " , " << floor(i/config_.width) << " : " << static_cast<unsigned int>(disparity);

        context_.tracer().debug( ss.str(), 9 );
        outimg[i] = disparity;
    }
}

} // namespace

extern "C" {
    hydrointerfaces::DisparityFactory *createDriverFactory()
    { return new disparitysimple::Factory; }
}
