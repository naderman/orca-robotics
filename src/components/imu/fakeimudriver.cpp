/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#include <iostream>
#include <orcaice/orcaice.h>

#include "fakeimudriver.h"

using namespace std;

namespace imu {

FakeImuDriver::FakeImuDriver()
{
}

FakeImuDriver::~FakeImuDriver()
{
    disable();
}

int 
FakeImuDriver::read()
{
    cout<<"TRACE(fakeimudriver.cpp): Generating fake imu data..." << endl;
    newImuData_ = false;
    orcaice::setToNow( imuData_.timeStamp );
    
    imuData_.accel.x = -35.0;
    imuData_.accel.y = 149.0;
    imuData_.accel.z = 50.0;
    imuData_.gyro.x  = 0.0;
    imuData_.gyro.y  = 0.0;
    imuData_.gyro.z  = 0.0;

    newImuData_ = true;
    
    newOdometry3dData_ = false;
    orcaice::setToNow( odometry3dData_.timeStamp );
    
    // position
    odometry3dData_.pose.p.x = 1;
    odometry3dData_.pose.p.y = 2;
    odometry3dData_.pose.p.z = 3;
    
    // attitude
    odometry3dData_.pose.o.r = 4;
    odometry3dData_.pose.o.p = 5;
    odometry3dData_.pose.o.y = 6;
    
    // velocity
    odometry3dData_.motion.v.x = 7;
    odometry3dData_.motion.v.y = 8;
    odometry3dData_.motion.v.z = 9;
    odometry3dData_.motion.w.x = 10;
    odometry3dData_.motion.w.y = 11;
    odometry3dData_.motion.w.z = 12;
    
    newOdometry3dData_ = true;
    
    IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(1));
    return 0;
}

int 
FakeImuDriver::getData(orca::ImuData& data )
{
    if(newImuData_){
	newImuData_ = false;
	data = imuData_;
        return 0;
    }else{
        return -1;
    }

}

int 
FakeImuDriver::getData(orca::Odometry3dData& data )
{
    if(newOdometry3dData_){
        newOdometry3dData_ = false;
        data = odometry3dData_;
        return 0;
    }else{
        return -1;
    }

}

} //namespace
