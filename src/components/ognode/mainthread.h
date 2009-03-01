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
#include <gbxsickacfr/gbxiceutilacfr/buffer.h>
#include <orcaice/context.h>

#include <orca/ogfusion.h>
#include <hydroogmap/hydroogmap.h>
#include <orcaifaceimpl/ogmap.h>

namespace ognode
{

class MainThread : public orcaice::SubsystemThread
{
public:
    MainThread( const orcaice::Context& context);

private:

    // from SubsystemThread
    virtual void initialise();
    virtual void work();
    
    void init();
    void setUpInternalMapFromPriorMap( const std::string &priorMapProxyString );
    void setUpInternalMapFromProperties();

    Ice::ObjectPtr                        ogFusionObjPtr_;
    orcaifaceimpl::OgMapImplPtr           ogMapImpl_;
    gbxiceutilacfr::Buffer<orca::OgFusionData> ogFusionDataBuffer_;

    // Represent the occupancy certainty values using doubles internally.
    hydroogmap::GenericMap<double> internalMap_;

    double maxPushPeriodSec_;

    orcaice::Context context_;
};

} // namespace

#endif
