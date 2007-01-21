/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef FAKE_INSGPS_DRIVER_H
#define FAKE_INSGPS_DRIVER_H

#include <iostream>

#include "driver.h"

namespace insgps{

//
// Bogus insgps driver, useful for testing the rest of the component.
//
class FakeInsGpsDriver : public Driver
{

public:
    
    FakeInsGpsDriver( const Config& cfg, const orcaice::Context& context );
    virtual ~FakeInsGpsDriver();

    // Initializes the device. If it's aleady initialized, then it
    // quietly re-initializes it.
    // returns: 0 = success, non-zero = failure
    virtual int init();   

    virtual int reset(){return 0;};

    virtual void enable();
    virtual int disable() { isEnabled_=false; return 0; };

    virtual bool isEnabled() { return isEnabled_; };

    // This is the thread's function.  It listens for data from the Novatel receiver,
    // and sticks it in the <novatelMessages>Proxy.
    virtual void run();

    // Fetch latest data from the hardware driver. return -1 if unavailable
    virtual void readGps(orca::GpsData& data, int timeoutMs=2000 );
    virtual void readGpsTime(orca::GpsTimeData& data, int timeoutMs=2000 );
    virtual void readImu(orca::ImuData& data, int timeoutMs=2000 );
    virtual void readOdometry3d(orca::Odometry3dData& data, int timeoutMs=2000 );
    
    virtual void shutdown();

private:

    // the driver will put the latest data into this queue buffer
    orcaice::Buffer<orca::GpsData> gpsDataBuffer_;
    orcaice::Buffer<orca::ImuData> imuDataBuffer_;
    orcaice::Buffer<orca::Odometry3dData> odometry3dDataBuffer_;

    // Blocks till timout expires, returns number of messages read, -1 if failure
    virtual int readMsgsFromHardware();
    
    bool isEnabled_;
    int numReads_;
    
    orcaice::Context context_;

    // for sending gps, imu, and odometry3d data at different times   
    int count_;   
};

//
// Exception thrown by FakeInsGpsDriver.
//
class FakeInsGpsException : public std::exception
{ 
    std::string  message_;
public:
    FakeInsGpsException(const char *message)
        : message_(message) {}
    FakeInsGpsException(const std::string &message)
        : message_(message) {}
    ~FakeInsGpsException()throw(){}
    virtual const char* what() const throw() { return message_.c_str(); }
};


} //namespace

#endif
