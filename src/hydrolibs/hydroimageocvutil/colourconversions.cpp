/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Ben Upcroft, Alex Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
 
#include <sstream>
#include <iostream>
#include "colourconversions.h"
// #include "conversions.h"

using namespace std;

namespace hydroimageocvutil{
 
void 
cvtToRgb( const IplImage* src, IplImage* dest, const std::string format )
{
    if ( format == "RGB8" )
    {
        // do nothing
        dest->imageData = src->imageData;
        // memcpy( dest->imageData, &src->data[0], src->description->size );
    }
    else
    {
        cout<<"ERROR(colourconversions.cpp): Don't know how to convert iamge format into RGB8." << endl;
        exit(1);
    }
}

void 
cvtToBgr( const IplImage* src, IplImage* dest, const std::string format )
{
    if( format == "BGR8" )
    {
        // do nothing but copy data into destination array.
        // Need to do the copy just in case the src image is being manipulated
        // and we don't want those manipulations to occur on the destination image
        // as well
        cvCopy(src, dest);

    }
    else if( format == "RGB8" )
    {
        cvCvtColor( src, dest, CV_RGB2BGR );
    }
    else if( format == "BGRA8" )
    {
        cvCvtColor( src, dest, CV_BGRA2BGR );
    }
    else if( format == "RGBA8" )
    {
        cvCvtColor( src, dest, CV_BGRA2BGR );
    }
    else if( format == "BayerRG8" )
    {
        cvCvtColor( src, dest, CV_BayerRG2BGR );
    }
    else if( format == "BayerGR8" )
    {
        cvCvtColor( src, dest, CV_BayerGR2BGR );
    }
    else if( format == "BayerGB8" )
    {
        cvCvtColor( src, dest, CV_BayerGB2BGR );
    }
    else if( format == "BayerBG8" )
    {
        cvCvtColor( src, dest, CV_BayerBG2BGR );
    }
    else if( format == "GRAY8" )
    {
        cvCvtColor( src, dest, CV_GRAY2BGR );
    }
    else
    {
        cout << "ERROR(colourconversions.cpp): Don't know how to convert image format into BGR8." << endl;
        //TODO: throw an exception
        exit(1);
    }
}

void
dc1394ToBgr( const IplImage* src, IplImage* dest, const std::string format )
{
	if( format == "RAW8" )
	{
		uint8_t *rgbbuf = NULL;
		rgbbuf = (uint8_t*) malloc( src->width * src->height * 3 );

        dc1394_bayer_decoding_8bit( (uint8_t*) src->imageData, rgbbuf, src->width, src->height, 
                DC1394_COLOR_FILTER_GRBG, DC1394_BAYER_METHOD_NEAREST );
	
		//Bayer pattern
// 		Definition at line 108 of file types.h.
// 
// 		00108              {
// 		00109     DC1394_COLOR_FILTER_RGGB = 512,
// 		00110     DC1394_COLOR_FILTER_GBRG,
// 		00111     DC1394_COLOR_FILTER_GRBG,
// 		00112     DC1394_COLOR_FILTER_BGGR
// 		00113 } dc1394color_filter_t;


        memcpy(dest->imageData, rgbbuf,src->width * src->height * 3 );
		free (rgbbuf);
        cvConvertImage( dest, dest, CV_CVTIMG_SWAP_RB );
	}
	else
	{
        cout << "ERROR(colourconversions.cpp): Don't know how to convert image format [" << format << " ] into BGR8." << endl;
        //TODO: throw an exception
        exit(1);
	}
}

} // namespace

