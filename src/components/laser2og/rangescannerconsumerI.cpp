/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#include <orcaice/orcaice.h>

#include "rangescannerconsumerI.h"

RangeScanner2dConsumerI::RangeScanner2dConsumerI(orcaice::PtrBuffer<orca::RangeScanner2dDataPtr> &RangeScanner2dDataBuffer)
    :rangeScannerDataBuffer_(RangeScanner2dDataBuffer)
{
}

void
RangeScanner2dConsumerI::setData(const orca::RangeScanner2dDataPtr& data, const Ice::Current&)
{
    /*
    try {
	// Is it a laser scan?
	std::cout << orca::LaserScanner2dDataPtr::dynamicCast( data ) << std::endl;
    }
    catch ( IceUtil::NullHandleException &e )
    {
	// Wasn't a laser scan.
	std::cout << data << std::endl;
    }
    std::cout << " (startAngle, angleIncrement: " << RAD2DEG(data->startAngle)
	<< ", " << RAD2DEG(data->angleIncrement) << ")"
	<< std::endl;
	*/
    rangeScannerDataBuffer_.push(data);
}
