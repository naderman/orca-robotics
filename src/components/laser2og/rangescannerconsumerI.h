#ifndef _LASER2OG_RANGESCANNER_CONSUMER_I_H_
#define _LASER2OG_RANGESCANNER_CONSUMER_I_H_

#include <iostream>

#include <orca/rangescanner.h>
#include <orcaice/ptrbuffer.h>

class RangeScannerConsumerI : public orca::RangeScannerConsumer
{
public:
    RangeScannerConsumerI(orcaice::PtrBuffer<orca::RangeScannerDataPtr> &RangeScannerDataBuffer);
    virtual void setData(const orca::RangeScannerDataPtr& data, const Ice::Current&);
private:
    orcaice::PtrBuffer<orca::RangeScannerDataPtr> &rangeScannerDataBuffer_;
};

#endif
