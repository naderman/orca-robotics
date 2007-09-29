/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef _ORCA2_OGNODE_HANDLER_H_
#define _ORCA2_OGNODE_HANDLER_H_

#include <orcaiceutil/thread.h>
#include <orcaiceutil/buffer.h>
#include <orcaice/context.h>

#include <orca/ogfusion.h>
#include <orcaogmap/orcaogmap.h>
#include <orcaifaceimpl/ogmapImpl.h>

namespace ognode
{

class Handler : public orcaiceutil::Thread
{
public:
    Handler( const orcaice::Context              &context);
    ~Handler();

    virtual void run();
private:

    void init();

    Ice::ObjectPtr                      ogFusionObjPtr_;
    orcaifaceimpl::OgMapImplPtr        ogMapImpl_;
    orcaiceutil::Buffer<orca::OgFusionData> ogFusionDataBuffer_;

    // Representt he occupancy certainty values using doubles internally.
    orcaogmap::GenericMap<double> internalMap_;

    orcaice::Context context_;
};

} // namespace

#endif
