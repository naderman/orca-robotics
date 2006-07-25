/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp,
 *               George Mathews, Stef Williams.
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_LASER_CONSUMER_I_H
#define ORCA_LASER_CONSUMER_I_H

#include <orca/laser.h>
#include <orcaice/ptrbuffer.h>

namespace laserfeatures
{

class LaserConsumerI : public orca::RangeScannerConsumer
{
    public:
        LaserConsumerI ( orcaice::PtrBuffer<orca::LaserDataPtr> &laserDataBuffer );
        
        virtual void setData(const orca::RangeScannerDataPtr& data, const Ice::Current&);
        
    private:
        orcaice::PtrBuffer<orca::LaserDataPtr> &laserDataBuffer_;
};

} // namespace

#endif
