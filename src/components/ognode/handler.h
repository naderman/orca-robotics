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

#include <orcaice/thread.h>
#include <orcaice/buffer.h>
#include <orcaice/context.h>

#include <orca/ogfusion.h>
#include <orcaogmap/orcaogmap.h>
#include <orcaifaceimpl/ogmapiface.h>

namespace ognode
{

class Handler : public orcaice::Thread
{
public:
    Handler( const orcaice::Context              &context);
    ~Handler();

    virtual void run();
private:

    void init();

    Ice::ObjectPtr                      ogFusionObjPtr_;
    orcaifaceimpl::OgMapIfacePtr        ogMapIface_;
    orcaice::Buffer<orca::OgFusionData> ogFusionDataBuffer_;

    // Representt he occupancy certainty values using doubles internally.
    orcaogmap::GenericMap<double> internalMap_;

    orcaice::Context context_;
};

} // namespace

#endif
