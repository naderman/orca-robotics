/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef ORCA2_IMAGE_GRABBER_H
#define ORCA2_IMAGE_GRABBER_H

#include <orca/camera.h>
#include <orcaice/context.h>      

namespace orcaimage {

/*
@brief Abstract class for grabbing images. This replaces the basic
parts of imagegrabber in libcamera used in Orca 1. Much of this interface has been copied from opencv highgui video i/o functions.

All image grabber implementations should implement this.

@author Ben Upcroft b.upcroft at cas.edu.au
*/

class ImageGrabber
{
//    const int SOME_ERROR = -1;
//    TODO: need replace -1 errors with exceptions
public:
    ImageGrabber( const orcaice::Context& context ) : context_(context) {};
    virtual ~ImageGrabber() {};

    // grabs frame from camera.
    // Warning: returns 0 on error, 1 on success!
    virtual int grabFrame() = 0;
    // load the raw image data grabbed from grabFrame() into a byte array.
    virtual char* retrieveFrame() = 0;
    // grabFrame() and retrieveFrame() in one.
    virtual char* queryFrame() = 0;

    // get and set properties
    virtual int width() const { return 0; };
    virtual void setWidth( int width ) {};
    virtual int height()  const { return 0; };
    virtual void setHeight( int height ) {};
    // image size in bytes
    virtual int size()  const { return 0; };

    // frames per second
    virtual double fps()  const { return 0.0; };
    virtual void setFps( double fps ) {};
    // enumerations of formats and modes exist in libdc1394/dc1394_control.h
    virtual int format()  const { return 0; };
    virtual void setFormat( int format ) {};
    virtual int mode()  const { return 0; };
    virtual void setMode( int mode ) {};
    virtual double brightness()  const { return 0.0; };
    virtual void setBrightness( double brightness ) {};
    virtual double contrast()  const { return 0.0; };
    virtual void setContrast( double contrast ) {};
    virtual double saturation()  const { return 0.0; };
    virtual void setSaturation( double saturation ) {};
    virtual double hue()  const { return 0.0; };
    virtual void setHue( int hue ) {};
    virtual double gain()  const { return 0.0; };
    virtual void setGain( int gain ) {};

    virtual double zoom() const { return -1.0; };
    virtual void setZoom( double zoom ) {};
    virtual double minZoom() const { return -1.0; };
    virtual double maxZoom() const { return -1.0; };

    virtual double focus() const {return -1.0; };
    virtual void setFocus( double zoom ) {};
    virtual double minFocus() const {return -1.0; };
    virtual double maxFocus() const {return -1.0; };

    virtual double shutter() const { return -1.0; };
    virtual void setShutter( double zoom ) {};
    virtual double minShutter() const { return -1.0; };
    virtual double maxShutter() const { return -1.0; };

protected:
    orcaice::Context context_;
   
};

}

#endif
