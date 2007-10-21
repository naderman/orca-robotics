/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#include "sickacfrdriver.h"
#include <iostream>
#include "messages.h"
#include <orcaice/orcaice.h>

using namespace std;

namespace sickacfr {

namespace {

    class NoResponseException : public std::exception
    {
        std::string  message_;
    public:
        NoResponseException(const char *message)
            : message_(message) {}
        NoResponseException(const std::string &message)
            : message_(message) {}
        ~NoResponseException()throw(){}
        virtual const char* what() const throw() { return message_.c_str(); }
    };

    class NackReceivedException : public std::exception
    {
        std::string  message_;
    public:
        NackReceivedException(const char *message)
            : message_(message) {}
        NackReceivedException(const std::string &message)
            : message_(message) {}
        ~NackReceivedException()throw(){}
        virtual const char* what() const throw() { return message_.c_str(); }
    };

    class ResponseIsErrorException : public std::exception
    {
        std::string  message_;
    public:
        ResponseIsErrorException(const char *message)
            : message_(message) {}
        ResponseIsErrorException(const std::string &message)
            : message_(message) {}
        ~ResponseIsErrorException()throw(){}
        virtual const char* what() const throw() { return message_.c_str(); }
    };
}

SickAcfrDriver::SickAcfrDriver( const laser2d::Driver::Config &config,
                                  const orcaice::Context &context )
    : config_(config),
      context_(context)
{
    // read driver-specific properties
    Ice::PropertiesPtr prop = context_.properties();
    std::string prefix = context_.tag()+".Config.SickAcfr.";

    baudRate_ =  orcaice::getPropertyAsIntWithDefault( prop, prefix+"Baudrate", 38400 );

    std::string serialDevice;
    int ret = orcaice::getProperty( prop, prefix+"Device", serialDevice );
    if ( ret != 0 )
    {
        stringstream ss;
        ss << "Property was not set: "<<(prefix+"Device");
        throw hydroutil::Exception( ERROR_INFO, ss.str() );
    }

    stringstream ssDebug;
    ssDebug << "Connecting to laser on serial port " << serialDevice;
    context_.tracer()->debug( ssDebug.str() );

    serialHandler_ = new SerialHandler( serialDevice, context_ );
    serialHandler_->start();

    try {

        initLaser();

    }
    catch ( ResponseIsErrorException &e )
    {
        std::string errorLog = errorConditions();
        stopAndJoin( serialHandler_ );
        stringstream ss;
        ss << e.what() << endl << "Laser error log: " << errorLog;
        throw ResponseIsErrorException( ss.str() );
    }
    catch ( std::exception &e )
    {
        stopAndJoin( serialHandler_ );
        throw;
    }
}

SickAcfrDriver::~SickAcfrDriver()
{
    // This deletes it
    stopAndJoin( serialHandler_ );
}

bool
SickAcfrDriver::waitForResponseType( uChar type, TimedLmsResponse &response, int maxWaitMs )
{
    orca::Time startWaitTime = orcaice::getNow();
    while ( true )
    {
        int ret = serialHandler_->responseBuffer().getAndPopNext( response, maxWaitMs );
        if ( ret == 0 )
        {
            double waitTimeSec = orcaice::timeDiffAsDouble( orcaice::getNow(), startWaitTime );

            // got response
            if ( response.response.type == type )
                return true;

            // Got a response, but not of the type we were expecting...
            //
            // In general, print out a warning message.
            // However, don't print out a warning if it's an ACK/NACK, because ACK/NACK don't
            // have checksums so they're likely to be found inside a failed-checksum message when a bit
            // gets flipped during transmission.
            if ( response.response.type != ACK &&
                 response.response.type != NACK )
            {
                stringstream ss;
                ss << "SickAcfrDriver::waitForResponseType(): While waiting for " << cmdToString(type) << ", received unexpected response: " << toString(response.response);
                context_.tracer()->warning( ss.str() );
            }

            if ( waitTimeSec > maxWaitMs/1000.0 )
            {
                return false;
            }
        }
        else if ( ret == -1 )
        {
            return false;
        }
        else
        {
            stringstream ss; ss << "Weird return code from getAndPopNext: " << ret;
            throw hydroutil::Exception( ERROR_INFO, ss.str() );
        }
    }
}

bool
SickAcfrDriver::waitForAckOrNack( bool &receivedAck )
{
    const int maxWaitMs = 1000;

    orca::Time startWaitTime = orcaice::getNow();
    while ( true )
    {
        TimedLmsResponse timedResponse;

        int ret = serialHandler_->responseBuffer().getAndPopNext( timedResponse, maxWaitMs );
        if ( ret == 0 )
        {
            // got timedResponse
            if ( timedResponse.response.type == ACK || timedResponse.response.type == NACK )
            {
                receivedAck = (timedResponse.response.type == ACK);
                return true;
            }
            else
            {
                stringstream ss;
                ss << "SickAcfrDriver::waitForAckOrNack(): Received unexpected response: " << toString(timedResponse.response);
                context_.tracer()->warning( ss.str() );
            }

            double waitTimeSec = orcaice::timeDiffAsDouble( orcaice::getNow(), startWaitTime );
            if ( waitTimeSec > maxWaitMs/1000.0 )
            {
                return false;
            }
        }
        else if ( ret == -1 )
        {
            context_.tracer()->debug( "SickAcfrDriver: waitForAckOrNack(): timed out.", 3 );
            return false;
        }
        else
        {
            stringstream ss;
            ss << "Weird return code from getAndPopNext: " << ret;
            throw hydroutil::Exception( ERROR_INFO, ss.str() );
        }
    }    
}

LmsResponse
SickAcfrDriver::askLaserForStatusData()
{
    constructStatusRequest( commandAndData_ );

    TimedLmsResponse response = sendAndExpectResponse( commandAndData_ );
    return response.response;
}

LmsResponse
SickAcfrDriver::askLaserForConfigData()
{
    constructConfigurationRequest( commandAndData_ );
    try {
        TimedLmsResponse response = sendAndExpectResponse( commandAndData_ );
        return response.response;
    }
    catch ( NoResponseException &e )
    {
        stringstream ss;
        ss << "While trying to askLaserForConfigData(): " << e.what();
        throw NoResponseException( ss.str() );
    }
}

uChar
SickAcfrDriver::desiredMeasuredValueUnit()
{
    if ( (int)(round(config_.maxRange)) == 80 )
    {
        return MEASURED_VALUE_UNIT_CM;
    }
    else if ( (int)(round(config_.maxRange)) == 8 )
    {
        return MEASURED_VALUE_UNIT_MM;
    }
    else
    {
        stringstream ss;
        ss << "Unknown linear resolution: " << config_.maxRange;
        throw hydroutil::Exception( ERROR_INFO, ss.str() );
    }
}

uint16_t
SickAcfrDriver::desiredAngularResolution()
{
    double angleIncrement = config_.fieldOfView / (config_.numberOfSamples-1);
    int angleIncrementInHundredthDegrees = (int)round(100.0 * angleIncrement*180.0/M_PI);

    assert( angleIncrementInHundredthDegrees == ANGULAR_RESOLUTION_1_0_DEG ||
            angleIncrementInHundredthDegrees == ANGULAR_RESOLUTION_0_5_DEG ||
            angleIncrementInHundredthDegrees == ANGULAR_RESOLUTION_0_25_DEG );
    return angleIncrementInHundredthDegrees;
}

bool 
SickAcfrDriver::isAsDesired( const LmsConfigurationData &lmsConfig )
{
    // This thing has a default constructor which will fill out most things
    return ( lmsConfig == desiredConfiguration() );
}

LmsConfigurationData
SickAcfrDriver::desiredConfiguration()
{
    // Default constructor sets up reasonable defaults which we then modify
    LmsConfigurationData c;
    
    c.measuringMode = MEASURING_MODE_8m80m_REFLECTOR8LEVELS;
    c.measuredValueUnit = desiredMeasuredValueUnit();

    // AlexB: It's not entirely clear, but from reading the SICK manual
    //        it looks like perhaps setting availability to 0x01 may
    //        help with dazzle (ie sunlight interfering with the laser).
    //        I haven't had a chance to test it though, so I'm not game
    //        to set it.
    cout<<"TRACE(sickacfrdriver.cpp): TODO: set availability?" << endl;
    // c.availability = 0x01;

    return c;
}

std::string
SickAcfrDriver::errorConditions()
{
    try {
        constructRequestErrorMessage( commandAndData_ );
        const bool ignoreErrorConditions = true;
        TimedLmsResponse errorResponse = sendAndExpectResponse( commandAndData_, ignoreErrorConditions );
        return toString( errorResponse.response );
    }
    catch ( std::exception &e )
    {
        stringstream ss;
        ss << "SickAcfrDriver: Caught exception while getting error conditions: " << e.what();
        context_.tracer()->debug( ss.str() );
        throw;
    }
}

void
SickAcfrDriver::setBaudRate( int baudRate )
{
    constructRequestBaudRate( commandAndData_, baudRate );
    sendAndExpectResponse( commandAndData_ );
    serialHandler_->setBaudRate( baudRate_ );
}

int
SickAcfrDriver::guessLaserBaudRate()
{
    // Guess our current configuration first: maybe the laser driver was re-started.
    std::vector<int> baudRates;
    baudRates.push_back( baudRate_ );
    if ( baudRate_ != 9600 ) baudRates.push_back( 9600 );
    if ( baudRate_ != 19200 ) baudRates.push_back( 19200 );
    if ( baudRate_ != 38400 ) baudRates.push_back( 38400 );
    if ( baudRate_ != 500000 ) baudRates.push_back( 500000 );
    
    for ( uint baudRateI=0; baudRateI < baudRates.size(); baudRateI++ )
    {
        stringstream ss;
        ss << "SickAcfrDriver: Trying to connect at " << baudRates[baudRateI] << " baud.";
        context_.tracer()->debug( ss.str() );

        serialHandler_->setBaudRate( baudRates[baudRateI] );
            
        const uint MAX_TRIES = 2;
        for ( uint tryNum=0; tryNum < MAX_TRIES; tryNum++ )
        {
            try {
                stringstream ss;
                ss <<"SickAcfrDriver: Trying to get laser status with baudrate " << baudRates[baudRateI] << " (try number "<<tryNum<<" of " << MAX_TRIES << ")" << endl;
                context_.tracer()->debug( ss.str() );
                askLaserForStatusData();
                return baudRates[baudRateI];
            }
            catch ( const NoResponseException &e )
            {
                stringstream ss;
                ss << "SickAcfrDriver::guessLaserBaudRate(): try " << tryNum << " of " << MAX_TRIES << "  at baudRate " << baudRates[baudRateI] << " failed: " << e.what();
                context_.tracer()->debug( ss.str() );
                if ( tryNum == MAX_TRIES-1 )
                    break;
            }
        }
    } // end loop over baud rates

    throw hydroutil::Exception( ERROR_INFO, "Failed to detect laser baud rate." );
}

void
SickAcfrDriver::initLaser()
{
    int currentBaudRate = guessLaserBaudRate();
    context_.tracer()->debug( "SickAcfrDriver: Guessed the baud rate!" );

    //
    // Turn continuous mode off
    //
    context_.tracer()->debug("SickAcfrDriver: Turning continuous mode off");
    constructRequestMeasuredOnRequestMode( commandAndData_ );
    sendAndExpectResponse( commandAndData_ );

    //
    // Set Desired BaudRate
    //
    if ( currentBaudRate != baudRate_ )
    {
        setBaudRate( baudRate_ );
    }

    // Gather info about the SICK
    stringstream ssInfo;
    ssInfo << "Laser info prior to initialisation:" << endl;

    //
    // Make note of error log
    //
    ssInfo << errorConditions() << endl;

    //
    // Check operating data counters
    //
    constructRequestOperatingDataCounter( commandAndData_ );
    TimedLmsResponse counterResponse = sendAndExpectResponse( commandAndData_ );
    ssInfo << "OperatingDataCounter: " << toString(counterResponse.response) << endl;

    //
    // Get status
    //
    LmsResponse statusResponse = askLaserForStatusData();
    LmsStatusResponseData *statusResponseData = 
        dynamic_cast<LmsStatusResponseData*>(statusResponse.data);
    assert( statusResponseData != NULL );
    ssInfo << "Status: " << statusResponseData->toString() << endl;

    LmsResponse configResponse = askLaserForConfigData();
    LmsConfigurationData *lmsConfig = 
        dynamic_cast<LmsConfigurationData*>(configResponse.data);
    assert( lmsConfig != NULL );
    ssInfo << "Config: " << configResponse.data->toString() << endl;

    context_.tracer()->info( ssInfo.str() );

    //
    // Enter installation mode
    //
    constructRequestInstallationMode( commandAndData_ );
    sendAndExpectResponse( commandAndData_ );

    //
    // Configure the thing if we have to
    //
    if ( !isAsDesired( *lmsConfig ) )
    {
        context_.tracer()->info( "SickAcfrDriver: Have to reconfigure the laser..." );

        constructConfigurationCommand( desiredConfiguration(),
                                       commandAndData_ );
        TimedLmsResponse configCmdResponse = sendAndExpectResponse( commandAndData_ );
        LmsConfigurationResponseData *configCmdResponseData = 
            dynamic_cast<LmsConfigurationResponseData*>(configCmdResponse.response.data);
        if ( !isAsDesired( configCmdResponseData->config ) )
        {
            stringstream ss;
            ss << "Error configuring SICK:  Config after configuration not what we expect.  Asked for: " << desiredConfiguration().toString() << endl << "got: " << configCmdResponseData->toString();
            throw hydroutil::Exception( ERROR_INFO, ss.str() );
        }
    }

    //
    // Configure the angular resolution
    //
    const uint16_t desiredScanningAngle = 180;
    constructSwitchVariant( desiredScanningAngle,
                            desiredAngularResolution(),
                            commandAndData_ );
    TimedLmsResponse angResponse = sendAndExpectResponse( commandAndData_ );
    LmsSwitchVariantResponseData *angResponseData =
        dynamic_cast<LmsSwitchVariantResponseData*>(angResponse.response.data);
    assert( angResponseData != NULL );
    if ( !( angResponseData->scanningAngle == desiredScanningAngle &&
            angResponseData->angularResolution == desiredAngularResolution() ) )
    {
            stringstream ss;
            ss << "Error configuring SICK variant:  Variant after configuration not what we expect: " << angResponseData->toString();
            throw hydroutil::Exception( ERROR_INFO, ss.str() );        
    }
    
    //
    // Start continuous mode
    //
    constructRequestContinuousMode( commandAndData_ );
    TimedLmsResponse contResponse = sendAndExpectResponse( commandAndData_ );

    context_.tracer()->info( "SickAcfrDriver: enabled continuous mode, laser is running." );
}

TimedLmsResponse
SickAcfrDriver::sendAndExpectResponse( const std::vector<uChar> &commandAndData, bool ignoreErrorConditions )
{
    constructTelegram( telegramBuffer_, commandAndData );

    assert( commandAndData.size()>0 && "constructTelegram should give telegram with non-zero size." );
    const uChar command = commandAndData[0];

    stringstream ss;
    ss << "SickAcfrDriver: requesting send "<<cmdToString(command)<<endl<<"  telegram: " 
       << toHexString(telegramBuffer_);
    context_.tracer()->debug( ss.str(), 3 );

    serialHandler_->send( telegramBuffer_ );

    bool isAck;
    bool receivedAckOrNack = waitForAckOrNack( isAck );
    if ( !receivedAckOrNack )
    {
        throw NoResponseException( "No ACK/NACK to command "+cmdToString(command) );
    }
    if ( !isAck )
    {
        throw NackReceivedException( "Received NACK for command "+cmdToString(command) );
    }

    int timeoutMs = 1000;
    if ( command == CMD_CONFIGURE_LMS )
    {
        // This takes a long time to complete
        timeoutMs = 15000;
    }

    TimedLmsResponse response;
    bool received = waitForResponseType( ack(command), response, timeoutMs );
    if ( !received )
    {
        throw NoResponseException( "No response to command "+cmdToString(command) );
    }

    if ( !ignoreErrorConditions && response.response.isError() )
    {
        throw ResponseIsErrorException( "Response contains an error condition: "+toString(response.response) );
    }

    return response;
}

void 
SickAcfrDriver::read( std::vector<float> &ranges, 
                       std::vector<unsigned char> &intensities, 
                       orca::Time &timeStamp )
{
    // This timeout is greater than the scan inter-arrival time for all baudrates.
    const int timeoutMs = 1000;

    TimedLmsResponse response;
    bool received = waitForResponseType( ACK_REQUEST_MEASURED_VALUES, response, timeoutMs );
    if ( !received )
    {
        throw hydroutil::Exception( ERROR_INFO, "No scan received." );
    }


    LmsMeasurementData *data = (LmsMeasurementData*)response.response.data;

    if ( response.response.isError() )
    {
        std::string errorLog = errorConditions();
        stringstream ss;
        ss << "Scan data indicates errors: " << toString(response.response) << endl << "Laser error log: " << errorLog;
        throw ResponseIsErrorException( ss.str() );        
    }

    memcpy( &(ranges[0]), &(data->ranges[0]), data->ranges.size()*sizeof(float) );
    memcpy( &(intensities[0]), &(data->intensities[0]), data->intensities.size()*sizeof(unsigned char) );
    timeStamp = response.timeStamp;
}

} // namespace

extern "C" {
    laser2d::DriverFactory *createDriverFactory()
    { return new sickacfr::SickAcfrDriverFactory; }
}
