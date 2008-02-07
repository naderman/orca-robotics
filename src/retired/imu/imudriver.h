/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef DRIVER_H
#define DRIVER_H

#include <orca/imu.h>
#include <orca/odometry3d.h>
#include <string>


static const ::std::string __orca__imu_default_heartbeat_msg = "";

namespace imu {

/*

@brief Abstract class for a imu.

All imui should implement this, making it simple
to add support for different imui.

This guy is _not_ guaranteed to be thread-safe!

@author Matthew Ridley, Ben Upcroft

*/
class ImuDriver
{

public:

    ImuDriver() {};
    virtual ~ImuDriver() {};

    virtual int reset()=0;

    virtual int enable()=0;
    virtual int disable()=0;

    virtual bool isEnabled()=0;

    // virtual bool hasFix()=0;

    // Blocks till timout expires, returns number of messages read, -1 if failure
    virtual int read()=0;
    // Fetch latest raw Imu data. return -1 if unavailable
    virtual int getData(orca::ImuData& data )=0;
    // Fetch latest Imu pva information. return -1 if unavailable
    virtual int getData(orca::Odometry3dData& data )=0;

    // Fetch Imu pva configuration information. return -1 if unavailable
//     virtual int getDescription(orca::Position3dDescription& descr )=0;

    // mechanism to get error messages etc back from driver.
    virtual const std::string &infoMessages() { return infoMessages_; };

    // Any special info to put in the heartbeat messages?
    virtual const std::string heartbeatMessage() { return __orca__imu_default_heartbeat_msg; };

protected:
    // set to false by call to getData()
    bool newImuData_;
    // set to false by call to getPvaData()
    bool newOdometry3dData_;

    orca::ImuData imuData_;
    orca::Odometry3dData odometry3dData_;

    std::string infoMessages_;

private:

};

} // namespace

#endif
