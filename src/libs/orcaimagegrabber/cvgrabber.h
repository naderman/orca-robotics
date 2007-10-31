/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef ORCA2_CV_GRABBER_H
#define ORCA2_CV_GRABBER_H

#include "imagegrabber.h"
#include <cv.h>
#include <highgui.h>

namespace orcaimage {

/*
@brief OpenCv implementation for grabbing images. 

@author Ben Upcroft b.upcroft at cas.edu.au
*/

    class CvGrabber : public ImageGrabber
    {
        
    public:
        CvGrabber( int cameraIndex, const orcaice::Context& context );
        virtual ~CvGrabber();

        // grabs frame from camera 
        virtual int grabFrame();
        // load the raw image data grabbed from grabFrame() into a byte array 
        virtual char* retrieveFrame();
        // grabFrame() and retrieveFrame() in one. You should normally call
        // this function rather than the other two directly.
        virtual char* queryFrame();

        // get and set properties that can be called for an opencv capture frame
        virtual int format() const;
        virtual void setFormat( int format );
        virtual int mode() const;
        virtual void setMode( int mode );
        virtual double fps() const; // frames per second
        virtual void setFps( double fps );

        // these set functions work for both v4l and dc_1394 but the
        // get functions only work for v4l
        virtual double brightness() const;
        virtual void setBrightness( double brightness );
        virtual double contrast() const;
        virtual void setContrast( double contrast );
        virtual double saturation() const;
        virtual void setSaturation( double saturation );
        virtual double hue() const;
        virtual void setHue( double hue );
        virtual double gain() const;
        virtual void setGain( double gain );

        // implemented inefficient workarounds to get these to work
        virtual int width() const;
        virtual void setWidth( int width );
        virtual int height() const;
        virtual void setHeight( int height );
        // image size in bytes
        virtual int size() const;

        // Things that aren't implemented in opencv yet.  Probably
        // could take the implementations of these functions from
        // libcamera.
        virtual double zoom() const { return -1.0; };
        virtual void setZoom( double zoom ) {};
        virtual double minZoom() const { return -1.0; };
        virtual double maxZoom() const { return -1.0; };

        virtual double focus() const { return -1.0; };
        virtual void setFocus( double zoom ) {};
        virtual double minFocus() const { return -1.0; };
        virtual double maxFocus() const { return -1.0; };

        virtual double shutter() const { return -1.0; };
        virtual void setShutter( double zoom ) {};
        virtual double minShutter() const { return -1.0; };
        virtual double maxShutter() { return -1.0; };

    private:
        // Video capturing structure
        CvCapture* capture_;

    };

}

#endif
