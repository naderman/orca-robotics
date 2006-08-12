#ifndef _BRUCE2_LASER2OG_HANDLER_H_
#define _BRUCE2_LASER2OG_HANDLER_H_

#include <orcaice/thread.h>
#include <orcaice/ptrbuffer.h>
#include <orcaice/context.h>

#include <orca/rangescanner.h>
#include <orca/localise2d.h>
#include <orca/ogfusion.h>

#include "laser2og.h"

namespace laser2og
{

class Handler : public orcaice::Thread
{
public:
    Handler(orcaice::PtrBuffer<orca::RangeScannerDataPtr> &RangeScannerDataBuffer,
		    const orca::Localise2dPrx  localise2dPrx,
		    const orca::OgFusionPrx  ogFusionPrx,
            Laser2Og& laser2Og,
		    orcaice::Context context );
    ~Handler();

    virtual void run();
private:
    // for the scans
    orcaice::PtrBuffer<orca::RangeScannerDataPtr> &rangeScannerDataBuffer_;

    // proxy for localise2d
    const orca::Localise2dPrx  localise2dPrx_;

    // proxy for OgFusion
    const orca::OgFusionPrx  ogFusionPrx_;

    Laser2Og& laser2Og_;

    orcaice::Context context_;

};

} //namespace

#endif

