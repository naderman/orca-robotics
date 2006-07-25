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

#include "laserconsumerI.h"

using namespace laserfeatures;

LaserConsumerI::LaserConsumerI( orcaice::PtrBuffer<orca::LaserDataPtr> &laserDataBuffer )
    : laserDataBuffer_(laserDataBuffer)
{

}

void LaserConsumerI::setData( const orca::RangeScannerDataPtr& data, const Ice::Current& )
{
    //cout << "INFO(laserconsumer_i.cpp): Received laserscan. Putting it into buffer now" << endl << endl;
    laserDataBuffer_.push( orca::LaserDataPtr::dynamicCast( data ) );
}
