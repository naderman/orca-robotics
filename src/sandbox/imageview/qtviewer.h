#ifndef IMAGEVIEW_QT_VIEWER_H
#define IMAGEVIEW_QT_VIEWER_H

#include "viewer.h"
// #include "imagequeue.h"
#include "qtviewerapp.h" 

// forward declaration
// class ImageQueue;

namespace imageview{

class QtViewer : public Viewer 
{
private:
    //! Private constructor so cannot be called directly other
    //! than by base class Viewer as it is a friend.
    //! This ensures that the QtViewer is created by the Viewer factory
    QtViewer( const orcaice::Context& context ) :
        Viewer(context)
    {
    }
    friend class Viewer;

    //! local endless queue for storing images received from communication buffers.
    // ImageQueue* imageQueue_;
    QtViewerApp* qtViewerApp_;

public:
    ~QtViewer();
    
    // from Viewer
    virtual void initialise();
    virtual void display( orca::ImageDataPtr image );
};

} //namespace

#endif
