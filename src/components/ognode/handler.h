#ifndef _BRUCE2_OGNODE_HANDLER_H_
#define _BRUCE2_OGNODE_HANDLER_H_

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
                  orcaice::Context current
                 );
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
