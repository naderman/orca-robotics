/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft,
 *                         Tom Burdick
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include "mainthread.h"

#include <iostream>
#include <memory>
#include <orcaice/orcaice.h>
#include <orcaobj/orcaobj.h> // for getPropertyAs...()

using namespace std;
using namespace disparity;

MainThread::MainThread( const orcaice::Context &context ) :
    orcaice::SubsystemThread( context.tracer(), context.status(), "MainThread" ),
    context_(context)
{
}

void
MainThread::initialise()
{
}

void
MainThread::work() 
{
}

void
MainThread::finalise() 
{
}

////////////////////

void
MainThread::readSettings()
{
}

void
MainThread::processData()
{
}

void
MainThread::initNetworkInterface()
{
}

void 
MainThread::initPluginInterface()
{
}


