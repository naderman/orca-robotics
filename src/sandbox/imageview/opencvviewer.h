#ifndef IMAGEVIEW_OPENCV_VIEWER_H
#define IMAGEVIEW_OPENCV_VIEWER_H

#include "viewer.h"

#include <highgui.h>
#include <cv.h>

namespace imageview{

class OpenCVViewer : public Viewer 
{
private:
    //! Private constructor so cannot be called directly other
    //! than by base class Viewer as it is a friend.
    //! This ensures that the QtViewer is created by the Viewer factory
    OpenCVViewer( const orcaice::Context& context );
    friend class Viewer;

    IplImage* cvImage_;
    // IplImage* cvImageTmp_;
    // IplImage* bayerImage_;
    const char* name_;

public:
    ~OpenCVViewer();
    
    // from Viewer
    virtual void initialise();
    virtual void display( orca::ImageDataPtr image );
};

} //namespace

#endif
