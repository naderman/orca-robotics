/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp,
 *               George Mathews, Stef Williams.
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_LASER_CONSUMER_I_H
#define ORCA_LASER_CONSUMER_I_H

#include <orca/laserscanner2d.h>
#include <orcaice/ptrproxy.h>

namespace laserfeatures
{

class LaserConsumerI : public orca::RangeScanner2dConsumer
{
    public:
        LaserConsumerI ( orcaice::PtrProxy<orca::LaserScanner2dDataPtr> &laserDataProxy );
        
        virtual void setData(const orca::RangeScanner2dDataPtr& data, const Ice::Current&);
        
    private:
        orcaice::PtrProxy<orca::LaserScanner2dDataPtr> &laserDataProxy_;
};

} // namespace

#endif
