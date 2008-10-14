/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Tom Burdick, Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef COMPONENT_H
#define COMPONENT_H


#include <orca/image.h>
#include <orcaice/component.h>
#include <orcaice/ptrbuffer.h>

namespace orcaimageview
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
