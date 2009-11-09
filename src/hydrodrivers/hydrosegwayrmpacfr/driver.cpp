/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <sstream>
#include <fstream>
#include <assert.h>
#include <IceUtil/IceUtil.h>    // for time functions
#include <hydroiceutil/timeutils.h>
#include <gbxutilacfr/mathdefs.h>
#include <gbxutilacfr/exceptions.h>

#include "driver.h"

// rmp data structure
#include "rmpdefs.h"
#include "canpacket.h"

// RMP IO Driver
#ifdef USB_IO_DRIVER
#   include "usbio/usbftdirmpio.h"
#elif CAN_IO_DRIVER
#   include "canio/canpeakrmpio.h"
#endif  

using namespace std;

namespace segwayrmpacfr
{

///////////////////////////////////////////////////////////////////

namespace 
{
    const int DEBUG_LEVEL=0;

    // Calculate the difference between two raw counter values, taking care of rollover.
    int32_t
    diffInclRollover( uint32_t from, uint32_t to )
    {
        int diff1, diff2;
        const uint32_t max = 0xFFFFFFFF;

        diff1 = to - from;

        // find difference in two directions and pick shortest
        if( to > from ) {
            diff2 = -(from + max - to);
        }
        else {
            diff2 = max - from + to;
        }

        if( abs(diff1) < abs(diff2) ) {
            return diff1;
        }
        else {
            return diff2;
        }
    }

    void
    logPacketToFile( const CanPacket &pkt, std::ofstream &file )
    {
        int seconds, useconds;
        hydroiceutil::now( seconds, useconds );

        file << seconds << ":" << useconds << " ";
        file << pkt.toString();
        file << endl;
    }

} // namespace

///////////////////////////////////////////////////////////////////

Driver::Driver( const std::string &powerbaseName, const hydroutil::Context &context )
    : odomX_(0),
      odomY_(0),
      odomYaw_(0),
      canDataFile_(0),
      powerbaseName_(powerbaseName),
      context_(context)
{    
    // Read configuration
    readFromProperties( context_, config_ );
    
    // Segway data "frame" transmitted over multiple packets
    rxData_.reset( new RxData( config_.model ) );
    
    // Init unit converter
    converter_.reset( new UnitConverter( config_.model ) );

#ifdef USB_IO_DRIVER
    context_.tracer().debug( powerbaseName_, "Driver: using USB/FTDI IO driver",3);
    rmpIo_.reset( new UsbFtdiRmpIo );
#elif CAN_IO_DRIVER
    context_.tracer().debug( powerbaseName_, "Driver: using CAN IO driver",3);
    // Get the port name that we are being asked to open (no default!)
    string portName;
    if( context_.properties().getProperty("Acfr.CanDevice", portName) !=0 ) {
        throw gbxutilacfr::Exception( ERROR_INFO, "Driver: config param Acfr.CanDevice not specified" );
    }
    rmpIo_.reset( new CanPeakRmpIo( portName ) );
#else
    throw hydrointerfaces::SegwayRmp::Exception( "Driver: cannot initialize an IO driver because preprocessor directives." );
#endif
    
    assert( rmpIo_.get() && "Must have an RMP IO object" );

    if ( config_.logCanDataToFile )
    {
        canDataFile_.reset( new ofstream( config_.canDataLogFileName.c_str() ) );
        if ( !canDataFile_->is_open() ) {
            stringstream ssFile;
            ssFile << "Couldn't open CAN log file '"<<config_.canDataLogFileName<<"' for writing.";
            throw gbxutilacfr::Exception( ERROR_INFO, ssFile.str() );
        }
    }

    stringstream ss;
    ss << "Driver: Using config: " << config_;
    context_.tracer().info( powerbaseName_, ss.str() );
}

Driver::~Driver()
{
    if ( canDataFile_.get() )
    {
        canDataFile_->close();
    }
}

double
Driver::maxSpeed() const
{
    double maxVelocityUnscaled = 
        converter_->speedInMperS(RMP_MAX_TRANS_VEL_COUNT);
    return config_.maxVelocityScale * maxVelocityUnscaled;
}

double
Driver::maxTurnrate( double speed ) const
{
    if ( speed < converter_->speedInMperS(RMP_MIN_SPEED_COUNTS_FOR_LATERAL_ACCEL_MODEL) )
    {
        const double maxTurnrateSlowSpeeds = RMP_MAX_ROT_VEL_RAD_PER_S;
        return maxTurnrateSlowSpeeds;
    }
    else
    {
        const double maxTurnrateByLateralAccLimit = RMP_MAX_LATERAL_ACCELERATION / speed;
        return maxTurnrateByLateralAccLimit;
    }
}

hydrointerfaces::SegwayRmp::Capabilities
Driver::capabilities() const
{
    hydrointerfaces::SegwayRmp::Capabilities c;

    c.maxForwardSpeed = this->maxSpeed();
    c.maxReverseSpeed = this->maxSpeed();
    c.maxTurnrate = this->maxTurnrate(0);
    c.maxLateralAcceleration = RMP_MAX_LATERAL_ACCELERATION;

    return c;
}

void
Driver::enable()
{
    rmpIo_->disable();

    // init device
    try {
        rmpIo_->enable( DEBUG_LEVEL );
    }
    catch ( std::exception &e )
    {
        stringstream ss;
        ss << "Driver: failed to enable.  Looks like the Segway is not connected.  Symptom is: "<<endl<<e.what();
        context_.tracer().error( powerbaseName_, ss.str() );
        throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
    }

    context_.tracer().debug( powerbaseName_, "Driver::enable(): connected to RmpIo device.");
    
    // segway is physically connected; try to configure

    // first, tell it to stand still.
    Command zero;
    zero.vx = 0.0;
    zero.w = 0.0;
    try {
        write( zero  );
    }
    catch ( std::exception &e )
    {
        stringstream ss; ss << "Driver::enable(): write failed: " << e.what();
        throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
    }

    // try reading from it (and fill out initial values in rxData_)
    try {
        // Do this a bunch, since the RMP seems to spit out all kinds
        // of crap when starting up (although this could be a Peak
        // problem)
        context_.tracer().debug( powerbaseName_, "Driver::enable(): Trying initial read" );
        for ( size_t i=0; i < 50; i++ )
        {
            *rxData_ = readData();
        }
    }
    catch ( std::exception &e )
    {
        stringstream ss;
        ss << "Driver: Looks like the Segway is powered off.  Symptom is: " << endl << e.what();
        throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
    }

    if ( config_.requireSpecificBuildId )
    {
        // Check Build ID (ie firmware version)
        if ( rxData_->buildId() != config_.requiredBuildId )
        {
            stringstream ss;
            ss << "Driver::enable(): RMP firmware version mismatch!" << endl
               << "  Required version: " << config_.requiredBuildId << endl
               << "  Found version:    " << rxData_->buildId();
            throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
        }
    }
         
    try {
        // Initialise everything
        resetAllIntegrators();
        setMaxVelocityScaleFactor( config_.maxVelocityScale );
        setMaxTurnrateScaleFactor( config_.maxTurnrateScale );
        setMaxAccelerationScaleFactor( config_.maxAccelerationScale );
        setMaxCurrentLimitScaleFactor( config_.maxCurrentLimitScale );
        setGainSchedule( config_.gainSchedule );
    }
    catch ( std::exception &e )
    {
        stringstream ss;
        ss << "Driver::enable() failed: " << e.what();
        rmpIo_->disable();
        throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
    }

    // Read once more to report the state
    try {
        *rxData_ = readData();
    }
    catch ( std::exception &e )
    {
        stringstream ss;
        ss << "Driver::"<<__func__<<"(): Problem with exploratory read: " << endl << e.what();
        throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
    }
    stringstream ssread;
    ssread << "Initial rxData: "<<endl<<rxData_->toString();
    context_.tracer().info( powerbaseName_, ssread.str() );
}

void
Driver::read( Data &data )
{
    try {
        //
        // Read a new RxData from the CAN bus
        //
        RxData newRxData = readData();

        // Calculate integrated odometry
        calculateIntegratedOdometry( *rxData_, newRxData );

        // Fill out the argument
        data = getData();

        // check for status changes
        if ( newRxData.rawData().status_word1 != rxData_->rawData().status_word1 || 
             newRxData.rawData().status_word2 != rxData_->rawData().status_word2 ) 
        {
            stringstream ss;
            ss << "Driver::"<<__func__<<"(): state changed.  State: "<<newRxData.toString();
            context_.tracer().info( powerbaseName_, ss.str() );
        }

        // update our stored rxData
        *rxData_ = newRxData;
    }
    catch ( std::exception &e )
    {
        stringstream ss; 
        ss << "Driver::read(): " << e.what();
        throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
    }
}

void
Driver::checkOperationalMode()
{
    if ( rxData_->operationalMode() == OperationalModeBalance &&
         !config_.allowMoveInBalanceMode )
    {
        throw gbxutilacfr::Exception( ERROR_INFO, "Driver: attempted to move in balance mode (disallowed by configuration)" );
    }
    if ( rxData_->operationalMode() == OperationalModeTractor &&
         !config_.allowMoveInTractorMode )
    {
        throw gbxutilacfr::Exception( ERROR_INFO, "Driver: attempted to move in tractor mode (disallowed by configuration)" );
    }
    if ( rxData_->operationalMode() == OperationalModeDisabled )
    {
        throw gbxutilacfr::Exception( ERROR_INFO, "Driver: attempted to move when OperationalMode=OperationalModeDisabled" );
    }    
}

void
Driver::applyScaling( const Command& originalCommand, Command &scaledCommand )
{
    scaledCommand.vx = originalCommand.vx / config_.maxVelocityScale;
    scaledCommand.w = originalCommand.w / config_.maxTurnrateScale;
}

void
Driver::write( const Command& command )
{
    if ( command.vx != 0 ||
         command.w  != 0 )
    {
        checkOperationalMode();
    }

    //
    // Scale to account for the funky scaling the RMP's gonna do
    //
    Command scaledCommand;
    applyScaling( command, scaledCommand );

    // Check if we're exceeding any limits
    if ( scaledCommand.vx > maxSpeed() )
    {
        stringstream ss;
        ss << "Driver::write: scaledCommand.vx(="<<scaledCommand.vx<<"m/s) was greater than maxSpeed(="<<maxSpeed()<<"m/s)..";
        context_.tracer().warning( powerbaseName_, ss.str() );
    }
    if ( scaledCommand.w > maxTurnrate(scaledCommand.vx) )
    {
        stringstream ss;
        ss << "Driver::write: scaledCommand.w(="<<scaledCommand.w*180.0/M_PI<<"deg/s) was greater than maxTurnrate at "<<scaledCommand.vx<<"m/s(="<<maxTurnrate(scaledCommand.vx)*180.0/M_PI<<"deg/s)..";
        context_.tracer().warning( powerbaseName_, ss.str() );
    }

    // Send the command to the RMP
    try {
        sendMotionCommandPacket( scaledCommand );
    }
    catch ( const std::exception &e )
    {
        stringstream ss;
        ss << "Driver::write(): failed to write command." << endl
           << "  command =       " << command.toString() << endl
           << "  scaledCommand = " << scaledCommand.toString() << endl
           << e.what();
        throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
    }
}

RxData
Driver::readData()
{
    RxData rxData( config_.model );

    RmpIo::RmpIoStatus status;
    int canPacketsProcessed = 0;
    int timeoutCount = 0;

    // arbitrary parameters
    const int maxCanPacketsProcessed = 100;
    const int maxTimeoutCount = 5;

    CanPacket pkt;

    while( canPacketsProcessed < maxCanPacketsProcessed && timeoutCount < maxTimeoutCount )
    {
        status = rmpIo_->readPacket( pkt );

        if ( status == RmpIo::NO_DATA ) 
        {
            // too many of these and we'll exit the loop with an error.
            ++timeoutCount;
            continue;
        }
        ++canPacketsProcessed;

        if ( config_.logCanDataToFile )
        {
            logPacketToFile( pkt, *canDataFile_ );
        }

        if ( pkt.id() == RMP_CAN_ID_HEARTBEAT )
        {
            // Ignore this, we can get all the info from other messages
            continue;
        }

        if ( pkt.id() == RMP_CAN_ID_SHUTDOWN )
        {
            stringstream ss;
            ss << "Driver::"<<__func__<<"(): Received Shutdown Message: this usually means that the UI box is turned on but the motor power is off.";
            throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
        }

        //
        // This is where we interprete all message contents!
        //
        rxData.addPacket(pkt);
   
        // If frame is complete, we can stop reading
        if ( rxData.isComplete() )
        {
            return rxData;
        }
    }   // while

    if ( canPacketsProcessed == maxCanPacketsProcessed )
    {
        stringstream ss;
        ss << "Driver::readData(): processed "<<canPacketsProcessed<<" CAN packets without finding all data.";
        throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
    }
    else if ( timeoutCount == maxTimeoutCount )
    {
        stringstream ss;
        ss << "Driver::readData(): timed out " << timeoutCount << " times while waiting for CAN data.";
        throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
    }
    else
    {
        throw gbxutilacfr::Exception( ERROR_INFO, "Driver::readData() failed for reason unknown (this shouldn't happen!)" );
    }
}

// AlexB: todo: We're getting yaw and foreAft direct from the Segway, how are these calculated?
//              Could we get them from the wheel positions instead?
void
Driver::calculateIntegratedOdometry( const RxData &prevData, const RxData &thisData )
{
    // Calculate the delta this new info represents
    int32_t deltaForeAftRaw = diffInclRollover( prevData.rawData().foreaft_displacement,
                                                thisData.rawData().foreaft_displacement );
    int32_t deltaYawRaw = diffInclRollover( prevData.rawData().yaw,
                                            thisData.rawData().yaw );

    // convert to SI
    double deltaForeAft = converter_->distanceInM( deltaForeAftRaw );
    double deltaYaw     = converter_->angleInRadiansFromRevCounts(deltaYawRaw);

    // First-order odometry integration
    odomX_   += deltaForeAft * cos(odomYaw_);
    odomY_   += deltaForeAft * sin(odomYaw_);
    odomYaw_ += deltaYaw;

    // Normalize yaw
    NORMALISE_ANGLE( odomYaw_ );
}

hydrointerfaces::SegwayRmp::Data
Driver::getData()
{
    hydrointerfaces::SegwayRmp::Data data;

    // set all time stamps right away
    hydroiceutil::now( data.seconds, data.useconds );

    // for x,y,yaw use locally-integrated values
    data.x     = odomX_;
    data.y     = odomY_;
    data.yaw   = odomYaw_;

    // Use what we can direct from the RMP
    data.roll        = rxData_->rollAngle();
    data.pitch       = rxData_->pitchAngle();
    data.droll       = rxData_->rollRate();
    data.dpitch      = rxData_->pitchRate();
    data.dyaw        = rxData_->yawRate();
    data.leftTorque  = rxData_->leftMotorTorque();
    data.rightTorque = rxData_->rightMotorTorque();
    data.mainvolt    = rxData_->baseBatteryVoltage();
    data.uivolt      = rxData_->uiBatteryVoltage();

    // combine left and right wheel velocity to get forward velocity
    data.vx = (rxData_->leftWheelVelocity() + rxData_->rightWheelVelocity()) / 2.0;

    data.hasWarnings = rxData_->isWarn();
    data.hasFaults = rxData_->isFault();
    if ( data.hasWarnings || data.hasFaults )
    {
        data.warnFaultReason = "State:\n" + rxData_->toString();
    }

    return data;
}

void
Driver::resetAllIntegrators()
{
    sendStatusCommandPacket( ConfigurationCommandResetIntegrators, 
                             RMP_CAN_RESET_ALL );
}

void
Driver::setMaxVelocityScaleFactor( double scale )
{
    assert( scale>=0.0);
    assert( scale<=1.0);

    sendStatusCommandPacket( ConfigurationCommandSetMaxVelocityScale,
                             converter_->maxVelocityScaleFactorAsRaw(scale) );
}

void
Driver::setMaxTurnrateScaleFactor( double scale )
{
    assert( scale>=0.0);
    assert( scale<=1.0);

    sendStatusCommandPacket( ConfigurationCommandSetMaxTurnrateScale,
                             converter_->maxTurnrateScaleFactorAsRaw(scale) );
}

void
Driver::setMaxAccelerationScaleFactor( double scale )
{
    assert( scale>=0.0);
    assert( scale<=1.0);

    sendStatusCommandPacket( ConfigurationCommandSetMaxAccelerationScale,
                             converter_->maxAccelerationScaleFactorAsRaw(scale) );
}

void
Driver::setMaxCurrentLimitScaleFactor( double scale )
{
    assert( scale>=0.0);
    assert( scale<=1.0);

    // note: the scale of this command is [0,256]
    sendStatusCommandPacket( ConfigurationCommandSetCurrentLimitScale,
                             converter_->maxCurrentLimitScaleFactorAsRaw(scale) );
}

void
Driver::setGainSchedule( int sched )
{
    sendStatusCommandPacket( ConfigurationCommandSetGainSchedule, sched );
}

void
Driver::enableBalanceMode( bool enable )
{
    if ( enable ) {
        sendStatusCommandPacket( ConfigurationCommandSetBalanceModeLockout, BalanceAllowed );
    }
    else {
        sendStatusCommandPacket( ConfigurationCommandSetBalanceModeLockout, BalanceNotAllowed );
    }
}

void
Driver::sendMotionCommandPacket( const Command& command )
{
    // translational RMP command
    int16_t trans = converter_->speedCommandAsRaw(command.vx);

    // rotational RMP command
    int16_t rot = converter_->angularRateCommandAsRaw(command.w);

    try {
        CanPacket pkt = motionCommandPacket( trans, rot );
        rmpIo_->writePacket( pkt );
        if ( config_.logCanDataToFile )
            logPacketToFile( pkt, *canDataFile_ );
    }
    catch ( std::exception &e )
    {
        stringstream ss;
        ss << "Driver::sendMotionCommandPacket(): " << e.what();
        throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
    }
}

/*
    Creates a status CAN packet from the given arguments
 */  
void
Driver::sendStatusCommandPacket( ConfigurationCommand command, uint16_t param )
{
    CanPacket pkt = statusCommandPacket( converter_->configurationCommandAsRaw( command ),
                                         param,
                                         (uint16_t)0,
                                         (uint16_t)0 );

    {
        stringstream ss;
        ss << "Driver: sending status command " << toString(command) << " with param " << param;
        context_.tracer().info( powerbaseName_, ss.str() );
    }

    try {
        rmpIo_->writePacket(pkt);
    }
    catch ( std::exception &e )
    {
        stringstream ss; ss << "Driver::sendStatusCommandPacket("<<toString(command)<<"): " << e.what();
        throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
    }
}

void
Driver::shutdown()
{
    context_.tracer().info( powerbaseName_, "Driver: sending SHUTDOWN command" );
    CanPacket pkt = shutdownCommandPacket();
    rmpIo_->writePacket(pkt);
}

} // namespace

extern "C" {
    hydrointerfaces::SegwayRmpFactory *createSegwayRmpDriverFactory()
    { return new segwayrmpacfr::Factory; }
}
