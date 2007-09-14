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

#include <orcaserial/serial.h>
#include <orcagpsutil/nmea.h>
#include <driver.h>

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
    virtual bool hasFix() { return hasFix_; };
    virtual void read();
    virtual int getData( orca::GpsData& data );
    virtual int getTimeData( orca::GpsTimeData& data );

private:

    void populateData();
    void enableDevice();
    void disableDevice();
    int resetDevice();
    void ExtractGGAData();
    void ExtractVTGData();
    void ExtractRMEData();

    orcaice::Context context_;
    
    orcaserial::Serial serial_;
    GpsProxy *gpsProxy_;
    orcagpsutil::NmeaMessage nmeaMessage_;
    IceUtil::Time timeOfRead_;

};

}

#endif
