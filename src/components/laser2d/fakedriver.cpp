/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#include "fakedriver.h"
#include <orcaice/orcaice.h>
#include <iostream>

using namespace std;

namespace sicklaser {

FakeDriver::FakeDriver()
    : currentConfig_(new orca::RangeScanner2dConfig)
{
}

FakeDriver::~FakeDriver()
{
    disable();
}

int 
FakeDriver::read( orca::LaserScanner2dDataPtr &data )
{
    cout<<"TRACE(fakelaserdriver.cpp): Generating fake laser data..." << endl;

    orcaice::setToNow( data->timeStamp );
    
    data->ranges.resize((int)(M_PI/0.5)); //currentConfig_->angleIncrement));
    data->intensities.resize(data->ranges.size());
    data->startAngle = (float)(DEG2RAD(-90));
    data->angleIncrement = 0.5; //currentConfig_->angleIncrement;

    for ( unsigned int i=0; i < data->ranges.size(); i++ )
    {
        data->ranges[i] = (float)((i/(float)(data->ranges.size()))*8.0);
        data->intensities[i] = i%2;
    }

    IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(1));
    return 0;
}

int 
FakeDriver::setConfig( const orca::RangeScanner2dConfigPtr &cfg )
{
    isEnabled_ = cfg->isEnabled;
    
    Ice::ObjectPtr super = cfg->ice_clone();
    currentConfig_ = orca::RangeScanner2dConfigPtr::dynamicCast( super );
    
    return 0;
}

int 
FakeDriver::getConfig( orca::RangeScanner2dConfigPtr &cfg )
{
    cfg = currentConfig_;
    return 0;
}

}
