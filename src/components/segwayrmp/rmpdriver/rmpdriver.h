/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_SEGWAY_RMP_DRIVER_H
#define ORCA2_SEGWAY_RMP_DRIVER_H

#include "../hwdriver.h"
#include "rmpdriverconfig.h"
#include "rmpio.h"
#include "rmpdataframe.h"
#include "canpacket.h"

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

    RmpDriver( const orcaice::Context & context,
               RmpIo &rmpIo );

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

    std::string toString();

    enum OperationalMode
    {
        OperationalModeTractor=1,
        OperationalModeBalance=2,
        OperationalModePowerdown=3
    };

    enum BalanceLockout
    {
        BalanceAllowed=0,
        BalanceNotAllowed=1
    };

    struct Status
    {
        int buildId;
        int cuState;
        int opMode;
        int gainSchedule;
        //OperationalMode opMode;
    };
    
    void setMaxVelocityScaleFactor( double scale );
    void setMaxTurnrateScaleFactor( double scale );
    void setMaxAccelerationScaleFactor( double scale );
    void setMaxCurrentLimitScaleFactor( double scale );
    void resetAllIntegrators();

    void setOperationalMode( OperationalMode mode );
    void setGainSchedule( int sched );
    void enableBalanceMode( bool enable );

    void applyScaling( const Command& original, Command &scaledCommand );

    // driver/hardware interface
    RmpIo         &rmpIo_;
    RmpDataFrame   frame_;
    CanPacket      pkt_;

    // configuration
    orcaice::Context context_;
    RmpDriverConfig  config_;

    // last motion commands [segway counts]
    // used to load into status command
    int16_t lastTrans_;
    int16_t lastRot_;

    // for integrating odometry
    uint32_t lastRawYaw_;
    uint32_t lastRawForeaft_;

    // for odometry calculation
    double odomX_;
    double odomY_;
    double odomYaw_;
    
    // for detecting internal state change
    int lastStatusWord1_;
    int lastStatusWord2_;

    void readFrame();
    void integrateMotion();
    void updateData( Data& data, Status & status );

    // helper to take a player command and turn it into a CAN command packet
    void makeMotionCommandPacket( CanPacket* pkt, const Command& command );
    void makeStatusCommandPacket( CanPacket* pkt, uint16_t commandId, uint16_t value );
    void makeShutdownCommandPacket( CanPacket* pkt );

    // Calculate the difference between two raw counter values, taking care of rollover.
    int diff(uint32_t from, uint32_t to, bool first);
    // bullshit
    bool firstread_;

    // chip's utilities
    void watchPacket( CanPacket* pkt, short int pktID );
    void watchDataStream( CanPacket* pkt );
};

} // namespace

#endif
