/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef _ORCA2_OGNODE_HANDLER_H_
#define _ORCA2_OGNODE_HANDLER_H_

#include <hydroutil/thread.h>
#include <hydroutil/buffer.h>
#include <orcaice/context.h>

#include <orca/ogfusion.h>
#include <hydroogmap/hydroogmap.h>
#include <orcaifaceimpl/ogmapImpl.h>

namespace ognode
{

class MainThread : public hydroutil::Thread
{
public:
    MainThread( const orcaice::Context              &context);
    ~MainThread();

    virtual void run();
private:

    void init();

    Ice::ObjectPtr                        ogFusionObjPtr_;
    orcaifaceimpl::OgMapImplPtr           ogMapImpl_;
    hydroutil::Buffer<orca::OgFusionData> ogFusionDataBuffer_;

    // Representt he occupancy certainty values using doubles internally.
    hydroogmap::GenericMap<double> internalMap_;

    orcaice::Context context_;
};

} // namespace

#endif
