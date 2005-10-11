/*
 *  Orca-Base: Components for robotics.
 *
 *  Copyright (C) 2004
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

#ifndef ORCA_DEVICE_USER_H
#define ORCA_DEVICE_USER_H

// include defnition of Ice runtime
#include <Ice/Ice.h>

#include <orca/device.h>

class DeviceUserI : public orca::DeviceUser
{
    public:
        virtual void acceptDataPush(const orca::SimpleLaserScan&, const Ice::Current&);
};

void DeviceUserI::acceptDataPush(const orca::SimpleLaserScan& scan, const Ice::Current&)
{
    std::cout<<"Received laser scan ("<<scan.returns.size()<<" returns) :";
    for ( uint i=0; i<scan.returns.size() && i<25; i+=5 ) {
        std::cout<<scan.returns[i].range<<":"<<(int)scan.returns[i].intensity<<" ";
    }
    std::cout<<std::endl;
}

#endif
