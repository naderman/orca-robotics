#ifndef IMAGEVIEW_OCV_VIEWER_H
#define IMAGEVIEW_OCV_VIEWER_H

// #include "viewer.h"

#include <orcaice/context.h>
#include <orca/image.h>

// opencv headers
#include <highgui.h>
#include <cv.h>

namespace imageviewocv {

class Viewer
{
public:
    Viewer( const orcaice::Context& context );
    ~Viewer();
    
//    void initialise();
    void display( orca::ImageDataPtr image );

private:
    
    IplImage* cvImage_;
    // IplImage* cvImageTmp_;
    // IplImage* bayerImage_;
    
    // handle to the opencv window
    const char* name_;

    orcaice::Context context_;
};

} //namespace

#endif
