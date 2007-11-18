/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_SEGWAY_RMP_DRIVER_H
#define ORCA2_SEGWAY_RMP_DRIVER_H

#include "../hwdriver.h"
#include "rmpdriverconfig.h"
#include "rmpio.h"
#include "rxdata.h"
#include "unitconverter.h"
#include "commandhistory.h"

namespace segwayrmp
{

//
// An implementation of HwHandler that knows how to talk Segway's
// protocol.  Requires a hardware interface implementation to be
// passed to the constructor.
//
// All public member functions throw RmpExceptions on error conditions.
//
class RmpDriver : public HwDriver
{
public:

    RmpDriver( const RmpDriverConfig  &driverConfig,
               RmpIo                  &rmpIo,
               const orcaice::Context &context );

    // Inherited from HwDriver
    virtual void enable();

    // Inherited from HwDriver
    virtual bool read( Data& data );

    // Inherited from HwDriver
    virtual void write( const Command& command );

    // Inherited from HwDriver
    virtual void getStatus( std::string &status, bool &isWarn, bool &isFault );
    
    // Modifies the arguments, thresholding to match what this hardware can actually do.
    void applyHardwareLimits( double &maxForwardSpeed, double &maxReverseSpeed, 
                              double &maxTurnrate, double &maxTurnrateAtMaxSpeed );

private:

    void setMaxVelocityScaleFactor( double scale );
    void setMaxTurnrateScaleFactor( double scale );
    void setMaxAccelerationScaleFactor( double scale );
    void setMaxCurrentLimitScaleFactor( double scale );
    void resetAllIntegrators();

    void setGainSchedule( int sched );
    void enableBalanceMode( bool enable );

    double maxSpeed();
    // max turnrate depends on the speed
    double maxTurnrate( double speed );

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
    const RmpDriverConfig config_;

    // driver/hardware interface
    RmpIo         &rmpIo_;

    // Remember the last commands we gave, so that we can check that the
    // received-commands reported by the Segway were actually sent.
    CommandHistory commandHistory_;

    // integrated odometry (in SI units)
    double odomX_;
    double odomY_;
    double odomYaw_;
    
    // Converts between units
    const UnitConverter converter_;

    // Most-recently-received data
    RxData rxData_;

    orcaice::Context context_;
};

} // namespace

#endif
