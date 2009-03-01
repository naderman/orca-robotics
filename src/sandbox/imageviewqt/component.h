/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Tom Burdick, Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_IMAGEVIEW_QT_COMPONENT_H
#define ORCA_IMAGEVIEW_QT_COMPONENT_H


#include <orca/image.h>
#include <orcaice/component.h>
#include <orcaice/ptrbuffer.h>

namespace imageviewqt
{

class ImageQueue;

class Component : public orcaice::Component
{
public:
    Component();
    ~Component();

    // component interface
    virtual void start();
    virtual void stop();

private:
    ImageQueue *imageQueue_;
    gbxiceutilacfr::ThreadPtr mainThread_;    
};

}


#endif
