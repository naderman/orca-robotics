/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef IMAGEVIEW_QTVIEWER_APP_H
#define IMAGEVIEW_QTVIEWER_APP_H

#include "viewwidget.h"
#include "imagequeue.h"

#include <gbxsickacfr/gbxiceutilacfr/thread.h>
#include <gbxutilacfr/tracer.h>

namespace imageview{

class QtViewerApp : public gbxiceutilacfr::Thread
{
public:
    QtViewerApp();
    ~QtViewerApp();

    // from Thread
    virtual void run();

    void push( orca::ImageDataPtr image );

private:
    //! local endless queue for storing images received from communication buffers.
    ImageQueue* imageQueue_;
    
    ViewWidget* viewer_;
    
};

} // namespace

#endif