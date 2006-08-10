#ifndef _BRUCE2_LASER2OG_COMPONENT_H_
#define _BRUCE2_LASER2OG_COMPONENT_H_


#include <orcaice/component.h>
#include <orcaice/ptrbuffer.h>
#include <orca/rangescanner.h>
#include <orca/ogfusion.h>
#include <orca/localise2d.h>

namespace laser2og 
{

class Handler;
class Laser2Og;

class Component : public orcaice::Component
{
public:
    Component();
    virtual ~Component();

    // component interface
    virtual void start();
    virtual void stop();
private:
    Handler *handler_;

    orcaice::PtrBuffer<orca::RangeScannerDataPtr> rangeScannerDataBuffer_;
    // for our requests for current location
    orca::Localise2dPrx localise2dPrx_;
    // for getting config data
    orca::RangeScannerPrx rangeScannerPrx_;
    // for setting ogfusion data
    orca::OgFusionPrx ogFusionPrx_;
    // for receiving the data
    orca::RangeScannerConsumerPrx rangeScannerConsumerPrx_;
    Laser2Og *OggerDumbPtr_;

};

}; // namespace

#endif
