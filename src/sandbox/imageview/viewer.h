/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Duncan Mercer, Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef IMAGEVIEW_VIEWER_H
#define IMAGEVIEW_VIEWER_H

// #include <gbxsickacfr/gbxiceutilacfr/buffer.h>
// #include <orcaice/ptrbuffer.h>
#include <orca/image.h>
#include <stdexcept>

namespace imageview {

// forward declarations
class QtViewer;
// class ImageQueue;

//! Actual base class for viewers
//! This class doesn't impose any interface other than creation of the actual
//! viewer
class Viewer
{
public:
    Viewer()//  : 
        // imageQueue_( 1, gbxiceutilacfr::BufferTypeCircular )
    {
    };
    virtual ~Viewer() {}
  
    // exceptions
    class BadViewerCreation : public std::logic_error 
    {
    public:
        BadViewerCreation(std::string type)
            : std::logic_error("Cannot create Viewer of type " + type) {}
    };

    //! Factory for viewers
    static Viewer* factory(const std::string& type) throw(BadViewerCreation);

    //! setup the viewer. 
    //! input argument = local image buffer where all the images are copied into
    virtual void initialise()=0;
    //! function that does all the displaying
    virtual void display( orca::ImageDataPtr image )=0;
    
protected:
    // pointer to local buffer holding images
    // gbxiceutilacfr::Buffer<orca::ImageDataPtr> imageQueue_;
    
    // the actual image we want to display
    // orca::ImageDataPtr image_;
};


} // namespace

// Used for dynamically loading driver
// extern "C" {
//     Viewer* createViewerFactory();
// }

#endif

