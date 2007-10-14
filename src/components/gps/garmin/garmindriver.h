/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Mathew Ridley, Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_GARMIN_GPS_DRIVER_H
#define ORCA2_GARMIN_GPS_DRIVER_H

#include <orcaice/orcaice.h>
#include <orcaserial/serial.h>
#include <orcagpsutil/nmea.h>
#include <driver.h>
#include <orca/gps.h>

namespace gps {

class GpsProxy;

/*
    @brief Interface to Garmin Gps
    @author Duncan Mercer
 */
class GarminGpsDriver : public Driver
{
public:


    GarminGpsDriver( const char             *device, 
                      const orcaice::Context &context);
    virtual ~GarminGpsDriver();

    virtual void init();

//! Read one complete frame of data
    virtual void read(orca::GpsData & gpsData) {return readFrame(gpsData );};

private:

    void addDataToFrame();
    void enableDevice();
    void disableDevice();
    int  resetDevice();
    void extractGGAData();
    void extractVTGData();
    void extractRMEData();
    void clearFrame(){haveGGA_ = false; haveVTG_ = false; haveRME_ =false;};
    bool haveCompleteFrame(){return (haveGGA_ && haveVTG_ && haveRME_);};
    
    void readFrame (orca::GpsData &gpsData);
    orcaice::Context context_;
    
    orcaserial::Serial serial_;
    GpsProxy *gpsProxy_;
    orcagpsutil::NmeaMessage nmeaMessage_;
    IceUtil::Time timeOfRead_;
    
    //NOTE if we change the number of messages in the frame, need to change
    //The N_MSGS_IN_FRAME in the readFrame fn..
    bool haveGGA_;
    bool haveVTG_;
    bool haveRME_;

};

}

#endif
