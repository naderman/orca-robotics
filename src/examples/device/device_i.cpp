/*
 *  Orca Project: Components for robotics.
 *
 *  Copyright (C) 2004-2006
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

#include <pthread.h>

#include "device_i.h"

void* storm_thread(void *pArg)
{
    DeviceI *device = (DeviceI*)pArg;

    //nice(5);
  
    std::cout << "Publishing thread started..." << std::endl;
    for (;;)
    {
        // not protecting scan_, it will crash eventually
        device->publish();
    }

    pthread_exit(NULL);
}

DeviceI::DeviceI( const orca::DeviceUserPrx & user ) :
        stormUser_(user), isPublishOn_(false), intervalMs_(1000)
{
    orca::LaserReturn ret;
    scan_.bearingStart = -3.14/2.0;
    scan_.bearingIncrement = 3.14/360.0;
    for ( uint i=0; i<361; ++i ) {
        ret.range = 5.0 + sin( (double)i / 6.28 );
        ret.intensity = 3;
        scan_.returns.push_back( ret );
    }

    pthread_t id_copy;
    int err = pthread_create( &id_copy, NULL, storm_thread, static_cast<void *>(this));
    if ( err != 0 ) {
        std::cerr << "*** Error starting publish thread *** : Error code " << err << std::endl;
        exit(1);
    }
}

void DeviceI::acceptDataPush(const orca::SimpleLaserScan& scan, const Ice::Current&)
{
    std::cout<<"Received laser scan ("<<scan.returns.size()<<" returns) :";
    for ( uint i=0; i<scan.returns.size() && i<25; i+=5 ) {
        std::cout<<scan.returns[i].range<<":"<<(int)scan.returns[i].intensity<<" ";
    }
    std::cout<<std::endl;
}

void DeviceI::subscribe(const orca::DeviceUserPrx& user, const Ice::Current&)
{
    std::cout<<"Received subscription request"<<std::endl;
    directUser_ = user;

    isPublishOn_ = true;
}

void DeviceI::setPushInterval(Ice::Int intervalMs, const ::Ice::Current&)
{
    std::cout<<"Changed push interval from "<<intervalMs_<<" to "<<intervalMs<<std::endl;
    intervalMs_ = intervalMs;
}

void DeviceI::publish()
{
    stormUser_->acceptDataPush( scan_ );
    IceUtil::ThreadControl::sleep(IceUtil::Time::milliSeconds(intervalMs_));
}

void DeviceI::requestData(const orca::DeviceUserPrx& user, const Ice::Current& current)
{
    std::cout << "Received data request" << std::endl;
    try
    {
        user->acceptDataPush( scan_ );
    }
    catch(const Ice::Exception& ex)
    {
        std::cout << ex << std::endl;
    }
}

orca::SimpleLaserScan DeviceI::getData(const orca::DeviceUserPrx& user, const Ice::Current& current)
{
    std::cout << "Sending data back" << std::endl;
    return scan_;
}

void DeviceI::shutdown(const Ice::Current& c)
{
    std::cout << "shutting down..." << std::endl;
    try
    {
        c.adapter->getCommunicator()->shutdown();
    }
    catch(const Ice::Exception& ex)
    {
        std::cout << ex << std::endl;
    }
}
