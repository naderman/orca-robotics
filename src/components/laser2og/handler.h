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
    
    Handler( const orcaice::Context &context );
    ~Handler();

    virtual void run();
    
private:

    void init();
    
    orcaice::Context context_;
    
    orcaice::PtrBuffer<orca::RangeScannerDataPtr> rangeScannerDataBuffer_;
    // for our requests for current location
    orca::Localise2dPrx localise2dPrx_;
    // for getting config data
    orca::RangeScannerPrx rangeScannerPrx_;
    // for setting ogfusion data
    orca::OgFusionPrx ogFusionPrx_;
    // for receiving the data
    orca::RangeScannerConsumerPrx rangeScannerConsumerPrx_;
    Laser2Og *laser2Og_;

};

} //namespace

#endif

