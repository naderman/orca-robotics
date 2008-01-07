/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Matthew Ridley, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_NOVATELSPAN_INSGPS_DRIVER_H
#define ORCA_NOVATELSPAN_INSGPS_DRIVER_H

#include "driver.h"
#include <orcaice/context.h>

// for reading and writing to a serial device
#include <hydroserial/serial.h>

// buffer for gps data
#include <hydroiceutil/buffer.h>

// novatel message definitions      
#include <hydronovatelutil/header.h>
#include <hydronovatelutil/log/rxstatus.h>
#include <hydronovatelutil/log/time.h>
#include <hydronovatelutil/log/timesync.h>
#include <hydronovatelutil/log/bestgpspos.h>
#include <hydronovatelutil/log/inspva.h>
#include <hydronovatelutil/log/inscov.h>
#include <hydronovatelutil/log/rawimu.h>


// binary messages defined by novatel
#pragma pack(push,1)
    typedef union novatel_message{
        struct{
            hydronovatelutil::OEM4_BINARY_HEADER hdr;
            char               data[484];
        };
        struct{
            hydronovatelutil::OEM4_SHORT_BINARY_HEADER short_hdr;
            char               short_data[500];
        };
        unsigned char raw_message[512];
    }novatel_message;
#pragma pack(pop)

namespace insgps{

/*
@brief Threaded class which handles interactions with the
       Novatel Span InsGps navigation sensors

@author Matthew Ridley, Ben Upcroft
 */
class NovatelSpanInsGpsDriver : public Driver
{
public:

    // Only serial device and baud rate are required
    NovatelSpanInsGpsDriver( const char *device, const int baud, const Config& cfg, const orcaice::Context& context );
    virtual ~NovatelSpanInsGpsDriver();

    virtual int reset();

     // Starts the thread and performs initial setup..
    // Throws NovatelSpanException if setup fails.
    virtual void enable();
    
    // returns: 0 = success, non-zero = failure
    virtual int disable();

    virtual bool isEnabled() { return enabled_; };
    // virtual bool hasFix() { return hasFix_; };

     // This is the thread's function.  It listens for data from the Novatel receiver, 
    // and sticks it in the <novatelMessages>Proxy.
    virtual void run();

    //
    // Blocking reads with default timeout of 2000ms. Also pops the front element from the buffer
    //
    
    // Read from the gpsBuffer where the driver is storing all incoming gps messages
    virtual void readGps( orca::GpsData& data, int timeoutMs=2000 );
    virtual void readImu(orca::ImuData& data, int timeoutMs=2000 );
    // Note that this driver only provides localise3d and not odometry3d.
    // This is here to keep the main driver happy as it is pure virtual
    virtual void readOdometry3d(orca::Odometry3dData& data, int timeoutMs=2000 );
    virtual void readLocalise3d(orca::Localise3dData& data, int timeoutMs=2000 );
    
    virtual void shutdown();
       
private:

    // send all the commands to the driver for setup and types of output messages.
     // You have to call start() before this will work.
    virtual int init();
       
    // copy novatel message info into the corresponding orca data buffers
    int populateData( int id );

    // read the actual messages from the Novatel receiver   
    virtual int readMsgsFromHardware();

    // read novatel specific messages and return message id so
    // we know what type of message to parse   
    int read_message( novatel_message* msg ); 
    
    std::string insStatusToString( const int& status );
    
    // convert back and forth between UTC or local time (expressed as a date/time string)
    // and GPS time (expressed as a number of seconds since the beginning of the GPS epoch,
    // 6 January 1980 0:00 UTC).
    int mkutctime(int week, double seconds, struct timeval* tv);

    // read from config file and set the Novatel-specific params 
    // (GPS Antenna to IMU offset, IMU orientation, vehicle to body rotation, etc. )
    void setNovatelSpecificParams();
    //orca::CartesianPoint imuToGpsAntennaOffset_;
    //orca::CartesianPoint imuToGpsAntennaOffsetUncertainty_;
    
    int baud_;

    // serial class for reading and writing to and from a serial device
    hydroserial::Serial* serial_;
    
    bool enabled_;
    bool swappedRollPitch_;
    // timestamp recorded after the first byte for accuracy
    IceUtil::Time timeOfRead_;
    
    // novatel binary messages
    novatel_message serial_data_;
    
    // novatel solution status
    unsigned long sol_status_;
    // novatel position type
    unsigned long pos_type_;

    // the driver will put the latest data into this queue buffer of depth 100
    hydroiceutil::Buffer<orca::GpsData> gpsDataBuffer_;
    hydroiceutil::Buffer<orca::ImuData> imuDataBuffer_;
    hydroiceutil::Buffer<orca::Odometry3dData> odometry3dDataBuffer_;
    hydroiceutil::Buffer<orca::Localise3dData> localise3dDataBuffer_;
        
    // Where the latest and greatest of each log is stored.
    // The trailing B's indicate binary messages
    // No trailing B means abbreviated ASCII message
    hydronovatelutil::RXSTATUSB_LOG  RXSTATUS_;
    hydronovatelutil::TIMEB_LOG      TIME_;
    hydronovatelutil::TIMESYNCB_LOG  TIMESYNC_;
    hydronovatelutil::BESTGPSPOS_LOG BESTGPSPOS_;
    hydronovatelutil::INSPVASB_LOG   INSPVA_;
    hydronovatelutil::INSCOVSB_LOG   INSCOV_;
    hydronovatelutil::RAWIMUSB_LOG   RAWIMU_;

    int gpsCount_;
    int imuCount_;
    int localise3dCount_;

#ifdef __QNX__
    // dynamically set priority of this thread in qnx
    int threadPriorityLow_;
    int threadPriorityHigh_;
#endif

    orcaice::Context context_;

};

//
// Exception thrown by NovatelSpanInsGpsDriver.
//
class NovatelSpanException : public std::exception
{ 
    std::string  message_;
public:
    NovatelSpanException(const char *message)
        : message_(message) {}
    NovatelSpanException(const std::string &message)
        : message_(message) {}
    ~NovatelSpanException()throw(){}
    virtual const char* what() const throw() { return message_.c_str(); }
};


} //namespace

#endif
