/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2008 Tom Burdick, Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef MAINTHREAD_H
#define MAINTHREAD_H


#include <orcaice/subsystemthread.h>
#include <orcaice/context.h>
#include <orcaifaceimpl/image.h>
#include <orca/image.h>

namespace orcaimageview
{
class ImageQueue;

class MainThread : public orcaice::SubsystemThread
{
public:
    MainThread( ImageQueue* imageQueue, const orcaice::Context& context );

    virtual void walk();

private:
    void init();

    //circular buffered consumer for recieving data
    orcaifaceimpl::BufferedImageConsumerImplPtr consumer_;

    // endless queue for display
    ImageQueue* imageQueue_;

    // context
    orcaice::Context context_;

};

}
#endif

