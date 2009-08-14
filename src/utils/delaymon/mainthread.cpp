/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/proputils.h>
#include "mainthread.h"

using namespace std;
using namespace delaymon;

MainThread::MainThread( const orcaice::Context &context ) : 
    orcaice::SubsystemThread( context.tracer(), context.status(), "MainThread" ),
    context_(context)
{
}

void
MainThread::initialise()
{
    Ice::PropertiesPtr prop = context_.properties();
    std::string prefix = context_.tag() + ".Config.";

    const int start = orcaice::getPropertyAsIntWithDefault( prop, prefix+"Start", 0 );
    const int end = orcaice::getPropertyAsIntWithDefault( prop, prefix+"End", 1000 );
    const int stride = orcaice::getPropertyAsIntWithDefault( prop, prefix+"Stride", 20 );
    const string fileprefix = orcaice::getPropertyWithDefault( prop, prefix+"FilenamePrefix", "histogram" );
    const int maxCount = orcaice::getPropertyAsIntWithDefault( prop, prefix+"MaxCount", -1 );

    consumer_ = new HistogramRangeScanner2dConsumerImpl( context_, start,end,stride, fileprefix, maxCount );
    // subscribe for data updates (multi-try)
    consumer_->subscribeWithTag( "LaserScanner2d", this, subsysName() );
}
