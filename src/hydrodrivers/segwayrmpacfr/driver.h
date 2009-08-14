/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef HYDRO_SEGWAYRMP_ACFR_H
#define HYDRO_SEGWAYRMP_ACFR_H

#include <hydrointerfaces/segwayrmp.h>
#include "rmpdriverconfig.h"
#include "rmpio.h"
#include "rxdata.h"
#include "unitconverter.h"

namespace segwayrmpacfr
{

//
// An implementation of HwHandler that knows how to talk Segway's
// protocol.  Requires a hardware interface implementation to be
// passed to the constructor.
//
// All public member functions throw hydrointerfaces::SegwayRmp::Exceptions on error conditions.
//
class Driver : public hydrointerfaces::SegwayRmp
{
public:

    Driver( const hydroutil::Context& context );
    virtual ~Driver();

    // from SegwayRmp
    virtual void enable();
    virtual void read( Data& data );
    virtual void write( const Command& command );
    virtual hydrointerfaces::SegwayRmp::Capabilities capabilities() const;

    // Custom method
    // Shuts down the motors
    void shutdown();

private:

    void setMaxVelocityScaleFactor( double scale );
    void setMaxTurnrateScaleFactor( double scale );
    void setMaxAccelerationScaleFactor( double scale );
    void setMaxCurrentLimitScaleFactor( double scale );
    void resetAllIntegrators();

    void setGainSchedule( int sched );
    void enableBalanceMode( bool enable );

    double maxSpeed() const;
    // max turnrate depends on the speed
    double maxTurnrate( double speed ) const;

    void applyScaling( const Command& original, Command &scaledCommand );

    // Checks that we're allowed to command the RMP in this operational mode (balance/tractor).
    // Throws exceptions if we're trying to do the wrong thing.
    void checkOperationalMode();

    // Reads a set of CAN packets, returns the result
    RxData readData();
    void calculateIntegratedOdometry( const RxData &prevData, const RxData &thisData );
    // fills up the Data class (understood externally)
    Data getData();

    void sendMotionCommandPacket( const Command& command );
    void sendStatusCommandPacket( ConfigurationCommand commandId, uint16_t param );

    // The RMP reports its last received command.  Check that this is what we sent.
    void checkReceivedCommand( const RxData &rxData );

    // Configuration
    RmpDriverConfig config_;

    // driver/hardware interface
    RmpIo*         rmpIo_;

    // integrated odometry (in SI units)
    double odomX_;
    double odomY_;
    double odomYaw_;
    
    // Converts between units
    // stored as a pointer because it needs model number to be initialized
    UnitConverter* converter_;

    // Most-recently-received data
    // stored as a pointer because it needs model number to be initialized
    RxData* rxData_;

    // For logging CAN data for debugging
    std::ofstream *canDataFile_;

    hydroutil::Context context_;
};

// Used for dynamically loading driver
class Factory : public hydrointerfaces::SegwayRmpFactory
{
public:
    hydrointerfaces::SegwayRmp*
    createDriver( const hydroutil::Context &context ) const
    {
        return new Driver( context );
    }
};

} // namespace

// Used for dynamically loading driver
extern "C" {
    hydrointerfaces::SegwayRmpFactory *createDriverFactory();
}

#endif
