/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Mathew Ridley, Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#include <iostream>
#include <orcaice/orcaice.h>

#include "fakegpsdriver.h"

using namespace std;

namespace gps {

FakeGpsDriver::FakeGpsDriver(std::vector<double> &latitudes, std::vector<double> &longitudes)
    : latitudes_(latitudes),
      longitudes_(longitudes),
      numReads_(0),
      dataRecord_(0),
      fixCounter_(0)
{
    cout << "sizes: " << latitudes.size() << " " << longitudes.size() << endl;
}

FakeGpsDriver::~FakeGpsDriver()
{
}


void
FakeGpsDriver::read(orca::GpsData & data)
{
//     cout<<"TRACE(fakelaserdriver.cpp): Generating fake gps data..." << endl;
    
    orcaice::setToNow( gpsData_.timeStamp );
    if (longitudes_.size()>0) 
    {
        gpsData_.latitude = latitudes_[dataRecord_];
        gpsData_.longitude = longitudes_[dataRecord_];
        // every 20 seconds we switch to the next gps record
        // if at the end, start again
        if (numReads_ % 20 == 0)
        {
            if (dataRecord_==longitudes_.size()-1) {
                dataRecord_ = 0;
            } else {
                dataRecord_++;
            }
        }
    } else {
        gpsData_.latitude = -35.0;
        gpsData_.longitude = 150.0;
    }
        
    gpsData_.altitude = 50;
    double heading = gpsData_.heading + DEG2RAD(10);
    gpsData_.heading = heading; 
    gpsData_.speed = 0;
    gpsData_.climbRate = 0;
    gpsData_.satellites = 6;
    gpsData_.positionType = orca::GpsPositionTypeAutonomous;
    gpsData_.geoidalSeparation = 10;

    
    numReads_++;
        
    IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(1));

    data=gpsData_;

}


}
