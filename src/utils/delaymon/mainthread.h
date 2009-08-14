/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#ifndef MAIN_THREAD_H
#define MAIN_THREAD_H

#include <orcaice/subsystemthread.h>
#include <orcaice/context.h>
#include "histogramconsumerImpl.h"
#include <orca/rangescanner2d.h>

namespace delaymon
{

typedef HistogramConsumerImpl<orca::RangeScanner2d, orca::RangeScanner2dConsumer,
                        orca::RangeScanner2dDataPtr> HistogramRangeScanner2dConsumerImpl;
typedef IceUtil::Handle<HistogramRangeScanner2dConsumerImpl> HistogramRangeScanner2dConsumerImplPtr;

class MainThread : public orcaice::SubsystemThread
{
public:
    MainThread( const orcaice::Context &context );

private:
    // from SubsystemThread
    virtual void initialise();

    HistogramRangeScanner2dConsumerImplPtr consumer_;

    orcaice::Context context_;
};

} // namespace

#endif
