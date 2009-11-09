#include "driver.h"

using namespace lfecombined;

int main()
{
    hydroutil::Context *context = NULL;
    hydroscanutil::ScannerConfig *scannerConfig = NULL;

    new Driver( *scannerConfig, *context );

    hydrointerfaces::LaserFeatureExtractorFactory* factory = new Factory;
    double laserMaxRange=0, laserStartAngle=0, laserAngleIncrement=0;
    int laserNumReturns=0;
    factory->createDriver( laserMaxRange, laserStartAngle, laserAngleIncrement, laserNumReturns, *context );

    return 0;
}
