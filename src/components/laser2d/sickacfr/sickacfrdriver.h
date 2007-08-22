/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef SICK_ACFR_DRIVER_H
#define SICK_ACFR_DRIVER_H

#include <laser2dutil/driver.h>
#include <orcaice/context.h>
#include <sickacfr/serialhandler.h>
#include <sickacfr/messages.h>

namespace sickacfr {

//!
//! @author Alex Brooks
//!
class SickAcfrDriver : public laser2d::Driver
{

public: 

    SickAcfrDriver( const laser2d::Driver::Config &config,
               const orcaice::Context &context );
    ~SickAcfrDriver();

    void read( std::vector<float> &ranges, 
               std::vector<unsigned char> &intensities, 
               orca::Time &timeStamp );

private: 

    // Waits up to maxWaitMs for a response of a particular type.
    // Returns true iff it got the response it wanted.
    bool waitForResponseType( uChar type, TimedLmsResponse &response, int maxWaitMs );
    // Returns: true if ack or nack received.
    // (and sets receivedAck: true = ACK, false=NACK)
    bool waitForAckOrNack( bool &receivedAck );

    LmsResponse askLaserForStatusData();
    LmsResponse askLaserForConfigData();

    LmsConfigurationData desiredConfiguration();
    bool isAsDesired( const LmsConfigurationData &lmsConfig );

    int guessLaserBaudRate();

    // Connects to the laser, sets params, and starts continuous mode.
    void initLaser();

    TimedLmsResponse sendAndExpectResponse( const std::vector<uChar> &commandAndData,
                                            bool ignoreErrorConditions=false );

    std::string errorConditions();

    uChar desiredMeasuredValueUnit();
    uint16_t desiredAngularResolution();

    void setBaudRate( int baudRate );

    laser2d::Driver::Config config_;
    SerialHandler *serialHandler_;
    int baudRate_;

    std::vector<uChar> commandAndData_;
    std::vector<uChar> telegramBuffer_;

    orcaice::Context context_;
};

// Used for dynamically loading driver
class SickAcfrDriverFactory : public laser2d::DriverFactory
{
public:
    laser2d::Driver *createDriver( const laser2d::Driver::Config &config,
                                   const orcaice::Context &context ) const
        {
            return new SickAcfrDriver( config, context );
        }
};

} // namespace

// Used for dynamically loading driver
extern "C" {
    laser2d::DriverFactory *createDriverFactory();
}

#endif
