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

#include "laserconsumerI.h"

using namespace laserfeatures;

LaserConsumerI::LaserConsumerI( orcaice::PtrProxy<orca::LaserScanner2dDataPtr> &laserDataProxy )
    : laserDataProxy_(laserDataProxy)
{

}

void LaserConsumerI::setData( const orca::RangeScanner2dDataPtr& data, const Ice::Current& )
{
    //cout << "INFO(laserconsumer_i.cpp): Received laserscan. Putting it into proxy now" << endl << endl;
    laserDataProxy_.set( orca::LaserScanner2dDataPtr::dynamicCast( data ) );
}
