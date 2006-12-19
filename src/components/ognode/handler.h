/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef _ORCA2_OGNODE_HANDLER_H_
#define _ORCA2_OGNODE_HANDLER_H_

#include <orcaice/thread.h>
#include <orcaice/ptrbuffer.h>
#include <orcaice/context.h>

#include <orca/ogfusion.h>
#include <orcaogmap/orcaogmap.h>
#include <orcaogfusion/orcaogfusion.h>
#include "ogmapI.h"

namespace ognode
{

class Handler : public orcaice::Thread
{
public:
    Handler(OgMapI& OgMapInterface,
            orcaice::PtrBuffer<orca::OgFusionDataPtr> &OgFusionDataBuffer,
            orcaogmap::OgMap& localMap_,
            const orcaice::Context & context);
    ~Handler();

    virtual void run();
private:
    OgMapI& ogMapInterface_;

    orcaice::PtrBuffer<orca::OgFusionDataPtr> &OgFusionDataBuffer_;

    orcaogmap::OgMap& localMap_;

    orcaice::Context context_;
};

} // namespace

#endif
