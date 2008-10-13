/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Tom Burdick, Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include "component.h"
#include "mainthread.h"
#include "viewwidget.h"

#include <QApplication>
#include <QTimer>

using namespace orcaimageview;

Component::Component()
: orcaice::Component( "OrcaImageView" )
{
    imageQueue_ = new orcaice::PtrBuffer<orca::ImageDataPtr>( 10, gbxiceutilacfr::BufferTypeCircular );
}

Component::~Component()
{
    delete imageQueue_;
}

void
Component::start()
{

    //create the event loop
    int argc = 0;
    char **argv = 0;
    
    QApplication app(argc, argv);

    QTimer *timer = new QTimer();
    timer->start(10); //start timer with an update of 10ms

    // create and show viewer
    ViewWidget* viewer = new ViewWidget(imageQueue_);
    viewer->show();

    // connect signal from timer to slot of widget to update
    QObject::connect(timer, SIGNAL(timeout()), viewer, SLOT(updateGL()));

    // start network thread
    mainThread_ = new MainThread(imageQueue_, context() );
    mainThread_->start();


    // start QApplication event loop
    app.exec();
    
    //app loop is done, delete gui objects
    delete timer;
    delete viewer;
  
    // normally ctrl-c handler does this, now we have to because UserThread keeps the thread
    context().communicator()->shutdown();
}

void 
Component::stop()
{
    gbxiceutilacfr::stopAndJoin( mainThread_ );
}
