/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
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
