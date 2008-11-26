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
    OpenCVViewer();
    friend class Viewer;

    //! local endless queue for storing images received from communication buffers.
    // ImageQueue* imageQueue_;
    // QtViewerApp* qtViewerApp_;

    IplImage* cvImage_;
    // IplImage* cvImageTmp_;
    // IplImage* bayerImage_;
    const char* name_;

    // orca::ImageData image_;

public:
    ~OpenCVViewer();
    
    // from Viewer
    virtual void initialise();
    virtual void display( orca::ImageDataPtr image );
};

} //namespace

#endif
