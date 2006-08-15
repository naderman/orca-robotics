/*
 *  Orca Project: Components for robotics.
 *
 *  Copyright (C) 2004-2006 Ben Upcroft
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
 
#include <iostream>
#include <orcaice/orcaice.h>

#include "fakeimudriver.h"

using namespace std;

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
    cout<<"TRACE(fakelaserdriver.cpp): Generating fake imu data..." << endl;
    newImuData_ = false;
    orcaice::setToNow( ImuData_.timeStamp );
    
    ImuData_.accelX = -35.0;
    ImuData_.accelY = 149.0;
    ImuData_.accelZ = 50;
    ImuData_.gyroX = 0;
    ImuData_.gyroY = 0;
    ImuData_.gyroZ = 0;

    newImuData_ = true;
    
    IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(1));
    return 0;
}

int 
FakeImuDriver::getData(orca::ImuDataPtr &data )
{
    if(newImuData_){
	newImuData_=false;
	*data=ImuData_;
        return 0;
    }else{
        return -1;
    }

}



