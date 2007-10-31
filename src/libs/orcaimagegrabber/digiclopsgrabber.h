/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef ORCA2_DIGICLOPS_GRABBER_H
#define ORCA2_DIGICLOPS_GRABBER_H

#include "imagegrabber.h"
#include "digiclops/digiclops.h"
#include "triclops/triclops.h"

#include <vector>

namespace orcaimage {

/*
@brief Wrapper for grabbing images using the the digiclops library. 

@author Ben Upcroft b.upcroft at cas.edu.au
*/

    class DigiclopsGrabber : public ImageGrabber
    {
        
    public:
        DigiclopsGrabber( const orcaice::Context& context );
        virtual ~DigiclopsGrabber();

        // grabs frame from camera 
        virtual int grabFrame();
        // load the image grabbed from grabFrame() into a byte array 
        virtual char* retrieveFrame();
        // grabFrame() and retrieveFrame() in one.
        virtual char* queryFrame();

        // get and set properties
        virtual int format() const;
        virtual void setFormat( int format );
        virtual int mode() const;
        virtual void setMode( int mode );
        virtual double fps() const; // frames per second
        virtual void setFps( double fps );

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

        virtual int width() const;
        // this set function automatically also sets the height since there are
        // only 2 enumerated types which set the resolution
        virtual void setWidth( int width );
        virtual int height() const;
        // this set function automatically also sets the height since there are
        // only 2 enumerated types which set the the resolution
        virtual void setHeight( int height );
        // image size in bytes
        virtual int size() const;

        // Not implemented yet. 
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
        DigiclopsContext digiclopsContext_;

        // Triclops image structure to access raw data
        TriclopsInput triclopsInput_;
        // raw data
        std::vector<unsigned char> data_; 
        // raw image data size
        int imageSize_;

        // digiclops errors
        DigiclopsError error_;

    };

}

#endif
