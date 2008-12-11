/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Ben Upcroft, Alex Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
 
#include <sstream>
#include "colourconversions.h"
#include "conversions.h"

using namespace std;

namespace cameracollectionviewocv{
 
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
        // do nothing
        dest->imageData = src->imageData;
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
        // do nothing
        // memcpy( dest->imageData, &(src->data[0]), src->data.size() );
        dest->imageData = src->imageData;
    }
    else
    {
        cout << "ERROR(colourconversions.cpp): Don't know how to convert image format into BGR8." << endl;
        //TODO: throw an exception
        exit(1);
    }
}


} // namespace

