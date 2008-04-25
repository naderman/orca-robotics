#include "rangescanpublisher.h"
#include <iostream>
#include <orcaice/orcaice.h>

using namespace std;

namespace orcasim2d {

void
RangeScanPublisher::publish( const std::vector<float> &ranges )
{
    orca::RangeScanner2dDataPtr orcaData = new orca::RangeScanner2dData;
    orcaData->timeStamp   = orcaice::getNow();
    orcaData->ranges      = ranges;
    orcaData->minRange    = descr_.minRange;
    orcaData->maxRange    = descr_.maxRange;
    orcaData->fieldOfView = descr_.fieldOfView;
    orcaData->startAngle  = descr_.startAngle;
    rangeScanner2dImpl_->localSetAndSend( orcaData );
}

}

