/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Mathew Ridley, Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
 
#include <iostream>
#include <orcaice/orcaice.h>
#include <orcaobj/mathdefs.h>

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
    disable();
}

bool 
FakeGpsDriver::hasFix() 
{ 
    fixCounter_++;
    if (fixCounter_>=5) {
        return true;
    } else {
        return false; 
    }
}

int 
FakeGpsDriver::read()
{
//     cout<<"TRACE(fakelaserdriver.cpp): Generating fake gps data..." << endl;
    
    newGpsData_ = false;
    orcaice::setToNow( GpsData_.timeStamp );
    orcaice::setSane( GpsTimeData_.utcTime );
    if (longitudes_.size()>0) 
    {
        GpsData_.latitude = latitudes_[dataRecord_];
        GpsData_.longitude = longitudes_[dataRecord_];
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
        GpsData_.latitude = -35.0;
        GpsData_.longitude = 150.0;
    }
        
    GpsData_.altitude = 50;
    double heading = GpsData_.heading + DEG2RAD(10);
    GpsData_.heading = heading; 
    GpsData_.speed = 0;
    GpsData_.climbRate = 0;
    GpsData_.satellites = 6;
    GpsData_.positionType = orca::GpsPositionTypeAutonomous;
    GpsData_.geoidalSeparation = 10;
    newGpsData_ = true;
    
    newGpsTime_ = false;
    orcaice::setToNow( GpsTimeData_.timeStamp );
    orcaice::setSane( GpsTimeData_.utcTime );
    orcaice::setSane( GpsTimeData_.utcDate );
    newGpsTime_ = true;
    
    numReads_++;
        
    IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(1));
    return 0;
}

int 
FakeGpsDriver::getData( orca::GpsData& data )
{
    if (newGpsData_)
    {
	   newGpsData_=false;
	   data=GpsData_;
       return 0;
    }
    return -1;
}

int 
FakeGpsDriver::getTimeData( orca::GpsTimeData& data )
{
    if (newGpsTime_)
    {
	   newGpsTime_=false;
	   data=GpsTimeData_;
       return 0;
    }
    return -1;
}

}
