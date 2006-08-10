#include <orcaice/orcaice.h>

#include "rangescannerconsumerI.h"

RangeScannerConsumerI::RangeScannerConsumerI(orcaice::PtrBuffer<orca::RangeScannerDataPtr> &RangeScannerDataBuffer)
    :rangeScannerDataBuffer_(RangeScannerDataBuffer)
{
}

void
RangeScannerConsumerI::setData(const orca::RangeScannerDataPtr& data, const Ice::Current&)
{
    /*
    try {
	// Is it a laser scan?
	std::cout << orca::LaserDataPtr::dynamicCast( data ) << std::endl;
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
