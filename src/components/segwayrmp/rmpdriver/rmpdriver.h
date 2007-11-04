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
    
    // Checks that the description matches what this hardware can actually do.
    void applyHardwareLimits( double& forwardSpeed, double& reverseSpeed, 
                              double& turnrate, double& turnrateAtMaxSpeed );

private:

    void setMaxVelocityScaleFactor( double scale );
    void setMaxTurnrateScaleFactor( double scale );
    void setMaxAccelerationScaleFactor( double scale );
    void setMaxCurrentLimitScaleFactor( double scale );
    void resetAllIntegrators();

    void setGainSchedule( int sched );
    void enableBalanceMode( bool enable );

    void applyScaling( const Command& original, Command &scaledCommand );

    // Reads a set of CAN packets, returns the result
    RxData readData();
    void calculateIntegratedOdometry( const RxData &prevData, const RxData &thisData );
    // fills up the Data class (understood externally)
    Data getData();

    CanPacket makeMotionCommandPacket( const Command& command );
    void sendStatusCommandPacket( ConfigurationCommand commandId, uint16_t param );

    // Configuration
    RmpDriverConfig  config_;

    // Which version of the RMP hardware are we using?
    RmpModel model_;

    // driver/hardware interface
    RmpIo         &rmpIo_;
    // RmpDataFrame   frame_;
    RxData         rxData_;

    // Remember the last command we gave, so that if we need to give a
    // configuration command (which also requires that a velocity be
    // specified) we can maintain the same speed.
    int16_t lastTrans_;
    int16_t lastRot_;

    // integrated odometry (in SI units)
    double odomX_;
    double odomY_;
    double odomYaw_;
    
    // Converts between units
    UnitConverter converter_;

    orcaice::Context context_;
};

} // namespace

#endif
