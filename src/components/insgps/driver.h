/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef DRIVER_H
#define DRIVER_H

// this class is a thread which inherits from thread.h
#include <hydroutil/safethread.h>

#include <orca/imu.h>
#include <orca/odometry3d.h>
#include <orca/localise3d.h>
#include <orca/gps.h>
#include <string>


static const ::std::string __orca__insgps_default_heartbeat_msg = "";

namespace insgps
{

/*

@brief Abstract class for a insgps.

All drivers should implement this, making it simple
to add support for different drivers.

This guy is _not_ guaranteed to be thread-safe!

@author Matthew Ridley, Ben Upcroft

*/

// GPS structures

typedef struct GpsPosition{
    struct timeval time;
    int zone;
    double northing;
    double easting;
    double down;
}GpsPosition;

typedef struct GpsStatus{
    struct timeval time;
    int    quality;
    int    sats;
    int    hdop;
    int    vdop;
}GpsStatus;

// pulse per second
typedef struct Pps{
    //utctime = gpstime + offset + utc_offset
    struct timeval time;          // utc
    struct timeval gpstime;       // gps
    double offset;
    double offset_std;
    double utc_offset;
}Pps;


class Driver : public hydroutil::Thread
{

public:

    class Config
    {   
    public:
        Config();
        bool validate() const;
        std::string toString() const;
        bool operator==( const Config & other );
        bool operator!=( const Config & other );
        
        orca::Frame2d gpsOffset;
        orca::Frame3d gpsAntennaOffset;
              
        orca::Frame3d imuOffset;
        orca::Size3d imuSize;                  
        
	orca::Frame3d vehiclePlatformToVehicleTransform;
    };
   
    Driver( const Config& cfg, const orcaice::Context& context ) : config_(cfg) {};
    virtual ~Driver() {};

    // This is the thread's function.
    virtual void walk(){ std::cout << "TRACE(driver::walk()): You must implement run for your derived driver " << std::endl; };

    // Initializes the device. If it's aleady initialized, then it
    // quietly re-initializes it.
    // returns: 0 = success, non-zero = failure
    virtual int init()=0;

    virtual int reset()=0;

    virtual void enable()=0;
    virtual int disable()=0;

    virtual bool isEnabled()=0;

    // virtual bool hasFix()=0;

    // Reads the actual messages from the hardware we're connected to   
    // Blocks till timout expires, returns number of messages read, -1 if failure
    virtual int readMsgsFromHardware()=0;
    
    // Read the latest data, return -1 if unavailable
    // Blocks with timeout, also quietly deletes the element that is read
    virtual void readGps( orca::GpsData& data, int timeoutMs=2000 ) = 0;
    virtual void readImu( orca::ImuData& data, int timeoutMs=2000 ) = 0;
    virtual void readOdometry3d( orca::Odometry3dData& data, int timeoutMs=2000 ) = 0;
    virtual void readLocalise3d( orca::Localise3dData& data, int timeoutMs=2000 ) = 0;

    // mechanism to get error messages etc back from driver.
    virtual const std::string &infoMessages() { return infoMessages_; };

    // Any special info to put in the heartbeat messages?
    virtual const std::string heartbeatMessage() { return __orca__insgps_default_heartbeat_msg; };

     // Simply returns the current stored configuration. Does not talk to device 
    Config config() const { return config_; };

    // the thread should be stopped and joined in this function   
    virtual void shutdown()=0;

protected:
    Pps pps_;
    GpsPosition position_;
    
    orca::GpsData gpsData_;
    orca::ImuData imuData_;
    orca::Odometry3dData odometry3dData_;
    orca::Localise3dData localise3dData_;

    std::string infoMessages_;
    
    Config config_;

    orcaice::Context context_;
   

private:

};

} //namespace

#endif
