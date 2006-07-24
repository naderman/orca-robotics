/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_LASERMON_LASER_CONSUMER_I_H
#define ORCA2_LASERMON_LASER_CONSUMER_I_H

#include <iostream>

#include <orca/laser.h>
#include <orcaice/orcaice.h>

class RangeScannerConsumerI : public orca::RangeScannerConsumer
{
    public:
        virtual void setData(const orca::RangeScannerDataPtr& data, const Ice::Current&)
        {
            try {
                // Is it a laser scan?
                //std::cout << orca::LaserDataPtr::dynamicCast( data ) << std::endl;
                std::cout << orcaice::toText( orca::LaserDataPtr::dynamicCast( data ) ) << std::endl;
            }
            catch ( IceUtil::NullHandleException & )
            {
                // Wasn't a laser scan.
                std::cout << data << std::endl;
            }
            std::cout << " (startAngle, angleIncrement: " << RAD2DEG(data->startAngle) 
                      << ", " << RAD2DEG(data->angleIncrement) << ")" 
                      << std::endl;
        }
};

#endif
