/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include "cvgrabber.h"

#include <iostream>

using namespace std;

namespace orcaimage {

CvGrabber::CvGrabber( int cameraIndex, const orcaice::Context& context )
    : ImageGrabber( context )
{
    context_.tracer()->debug("initializing image grabber",3);
//  CV_CAP_VFW      200   // platform native
//  CV_CAP_V4L      200
//  CV_CAP_V4L2     200

//  CV_CAP_FIREWARE 300   // IEEE 1394 drivers
//  CV_CAP_IEEE1394 300
//  CV_CAP_DC1394   300
//  CV_CAP_CMU1394  300

//  CV_CAP_ANY      0     // autodetect

    // capture_ = cvCaptureFromCAM( CV_CAP_ANY );
    capture_ = cvCaptureFromCAM( cameraIndex );
    if( capture_ ) {
        context_.tracer()->debug("CvGrabber initialised",3);
    } else {
        std::string s = "Failed to initialize image grabber";
        context_.tracer()->error(s);
        throw s;
    }

    // the default in opencv is CV_CAP_PROP_CONVERT_RGB=1 which causes
    // cameras with bayer encoding to be converted from mono to rgb
    // without using the bayer functions. CV_CAP_PROP_CONVERT_RGB=0
    // keeps the original format.
    cvSetCaptureProperty( capture_, CV_CAP_PROP_CONVERT_RGB, 0 );
};

CvGrabber::~CvGrabber()
{
    // Release the capture device
    cvReleaseCapture( &capture_ );
};

// grabs frame from camera 
int 
CvGrabber::grabFrame()
{
    if ( !cvGrabFrame( capture_ ) )
    {
        std::string s = "Failed to grab a frame from the camera.";
        context_.tracer()->error(s);
        return 0;
    }
    else
    {
        return 1;
    }
};


// load the raw image data grabbed from grabFrame() into a byte array
char* 
CvGrabber::retrieveFrame()
{
    IplImage* image = cvRetrieveFrame( capture_ );
    if ( !image )
    {
        std::string s = "Failed to retrieve image from the grabbed frame.";
        context_.tracer()->error(s);
        throw s;
    }
    else
    {
        return image->imageData;
    }
};

// grabFrame() and retrieveFrame() in one. You should normally call
// this function rather than the other two directly.
char* 
CvGrabber::queryFrame()
{
    IplImage* image = cvQueryFrame( capture_ );
    if ( !image )
    {
        std::string s = "Failed to query image from the grabbed frame.";
        context_.tracer()->error(s);
        throw s;
    }
    else
    {
         return image->imageData;
    }
};

// get and set properties for the grabbed frame
int CvGrabber::width() const
{
    // workaround for some of opencv grabber functionality...
    // load the grabbed frame into an iplimage struct and then find the width.
    IplImage* tmpImage = cvQueryFrame( capture_);
    int w = tmpImage->width;
    // is tmpImage deleted after this function?
    
    // this following call should work in opencv but have had mixed results
    // int width = (int)cvGetCaptureProperty( capture_, CV_CAP_PROP_FRAME_WIDTH );
    return w;
};

void CvGrabber::setWidth( int width )
{
    cvSetCaptureProperty( capture_, CV_CAP_PROP_FRAME_WIDTH, width );
    return;
};

int CvGrabber::height() const
{
    // workaround for some of opencv grabber functionality...
    // load the grabbed frame into an iplimage struct and then find the height.
    IplImage* tmpImage = cvQueryFrame( capture_);
    int h = tmpImage->height;
    // is tmpImage deleted after this function?
    
    // this following call should work in opencv but have had mixed results
    //int height = (int)cvGetCaptureProperty( capture_, CV_CAP_PROP_FRAME_HEIGHT );

    return h;
};

void CvGrabber::setHeight( int height )
{
    cvSetCaptureProperty( capture_, CV_CAP_PROP_FRAME_HEIGHT, height );
    return;
};

int CvGrabber::size() const
{
    // load the grabbed frame into an iplimage struct and then find the image size.
    IplImage* tmpImage = cvQueryFrame( capture_);
    int imageSize = tmpImage->imageSize;
    // is tmpImage deleted after this function?

    return imageSize;
};

// frames per second
double CvGrabber::fps() const
{
    double fps = cvGetCaptureProperty( capture_, CV_CAP_PROP_FPS );
    return fps;
};

void CvGrabber::setFps( double fps )
{
    cvSetCaptureProperty( capture_, CV_CAP_PROP_FPS, fps );
    return;
};

// enumerations of formats and modes exist in libdc1394/dc1394_control.h
int CvGrabber::format() const
{
    int format = (int)cvGetCaptureProperty( capture_, CV_CAP_PROP_FORMAT );
    return format;
};

void CvGrabber::setFormat( int format )
{
    cvSetCaptureProperty( capture_, CV_CAP_PROP_FORMAT, format );
    return;
};

int CvGrabber::mode() const
{
    int mode = (int)cvGetCaptureProperty( capture_, CV_CAP_PROP_MODE );
    return mode;
};

void CvGrabber::setMode( int mode )
{
    cvSetCaptureProperty( capture_, CV_CAP_PROP_MODE, mode );
    return;
};

double CvGrabber::brightness() const
{
    double brightness = cvGetCaptureProperty( capture_, CV_CAP_PROP_BRIGHTNESS );
    return brightness;
};

void CvGrabber::setBrightness( double brightness )
{
    cvSetCaptureProperty( capture_, CV_CAP_PROP_BRIGHTNESS, brightness );
    return;
};

double CvGrabber::contrast() const
{
    double contrast = cvGetCaptureProperty( capture_, CV_CAP_PROP_CONTRAST );
    return contrast;
};

void CvGrabber::setContrast( double contrast )
{
    cvSetCaptureProperty( capture_, CV_CAP_PROP_CONTRAST, contrast );
    return;
};

double CvGrabber::saturation() const
{
    double saturation = cvGetCaptureProperty( capture_, CV_CAP_PROP_SATURATION );
    return saturation;
};

void CvGrabber::setSaturation( double saturation )
{
    cvSetCaptureProperty( capture_, CV_CAP_PROP_SATURATION, saturation );
    return;
};

double CvGrabber::hue() const
{
    double hue = cvGetCaptureProperty( capture_, CV_CAP_PROP_HUE );
    return hue;
};

void CvGrabber::setHue( double hue )
{
    cvSetCaptureProperty( capture_, CV_CAP_PROP_HUE, hue );
    return;
};

double CvGrabber::gain() const
{
    double gain = cvGetCaptureProperty( capture_, CV_CAP_PROP_GAIN );
    return gain;
};

void CvGrabber::setGain( double gain )
{
    cvSetCaptureProperty( capture_, CV_CAP_PROP_GAIN, gain );
    return;
};

} // namespace
