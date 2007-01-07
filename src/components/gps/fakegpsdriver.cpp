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

#include "fakegpsdriver.h"

using namespace std;

FakeGpsDriver::FakeGpsDriver()
{
}

FakeGpsDriver::~FakeGpsDriver()
{
    disable();
}

int 
FakeGpsDriver::read()
{
    cout<<"TRACE(fakelaserdriver.cpp): Generating fake gps data..." << endl;
    newGpsData_ = false;
    orcaice::setToNow( GpsData_.timeStamp );
    GpsData_.latitude = -35.0;
    GpsData_.longitude = 149.0;
    GpsData_.altitude = 50;
    GpsData_.heading = 0;
    GpsData_.speed = 0;
    GpsData_.climbRate = 0;
    GpsData_.satellites = 6;
    GpsData_.positionType = 1;
    GpsData_.geoidalSeparation = 10;

    newGpsData_ = true;
    
    IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(1));
    return 0;
}

int 
FakeGpsDriver::getData( orca::GpsData& data )
{
    if(newGpsData_){
	newGpsData_=false;
	data=GpsData_;
        return 0;
    }else{
        return -1;
    }
}

int 
FakeGpsDriver::getTimeData( orca::GpsTimeData& data )
{
    if(newGpsTime_){
	newGpsTime_=false;
	data=GpsTimeData_;
        return 0;
    }else{
        return -1;
    }
}
