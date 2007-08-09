/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>

#include "fakedriver.h"

using namespace std;

namespace laser2d {

FakeDriver::FakeDriver( const Config &cfg, const orcaice::Context &context ) :
    config_(cfg),
    context_(context)
{
}

FakeDriver::~FakeDriver()
{
}

void
FakeDriver::read( std::vector<float> &ranges, 
                  std::vector<unsigned char> &intensities, 
                  orca::Time &timeStamp )
{
    context_.tracer()->info( "Generating fake laser data..." );

    orcaice::setToNow( timeStamp );
    
    for ( int i=0; i < config_.numberOfSamples; i++ )
    {
        ranges[i] = (float)((i/(float)(config_.numberOfSamples))*config_.maxRange);
        intensities[i] = i%2;
    }

    IceUtil::ThreadControl::sleep(IceUtil::Time::milliSeconds(100));

    timeStamp = orcaice::getNow();
}


} // namespace

extern "C" {
    laser2d::DriverFactory *createDriverFactory()
    { return new laser2d::FakeDriverFactory; }
}
