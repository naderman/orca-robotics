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

#include <orca/rangescanner2d.h>
#include <orcaice/ptrbuffer.h>

class RangeScanner2dConsumerI : public orca::RangeScanner2dConsumer
{
public:
    RangeScanner2dConsumerI(orcaice::PtrBuffer<orca::RangeScanner2dDataPtr> &RangeScanner2dDataBuffer);
    virtual void setData(const orca::RangeScanner2dDataPtr& data, const Ice::Current&);
private:
    orcaice::PtrBuffer<orca::RangeScanner2dDataPtr> &rangeScannerDataBuffer_;
};

#endif
