/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#include <iostream>
#include <orcaice/orcaice.h>
#include <laser2dutil/sickutil.h>

#include "sickcarmendriver.h"
#include "sick.h"
#include "carmen_conversion.h"

using namespace std;

namespace laser2d {

SickCarmenDriver::SickCarmenDriver( const Config           &cfg,
                                    const orcaice::Context &context )
    : config_(cfg),
      laser_(0),
      context_(context)
{
    // read driver-specific properties
    Ice::PropertiesPtr prop = context_.properties();
    std::string prefix = context_.tag()+".Config.SickCarmen.";

    baudrate_ =  orcaice::getPropertyAsIntWithDefault( prop, prefix+"Baudrate", 38400 );

    std::string device = orcaice::getPropertyWithDefault( prop, prefix+"Device", "/dev/ttyS0" );
    device_ = strdup(device.c_str());

    std::string laserType = orcaice::getPropertyWithDefault( prop, prefix+"LaserType", "LMS" );
    type_ = strdup(laserType.c_str());

    init();
}

SickCarmenDriver::~SickCarmenDriver()
{
    if ( laser_ ) delete laser_;
}

int
SickCarmenDriver::setupParams( double maxRange, int numberOfSamples, int baudrate )
{    
    //
    // first convert from generic configs to SICK-specific ones
    //

    double rangeResolution = sickutil::maxRange2RangeResolution( maxRange );
    double angleIncrement = sickutil::numberOfSamples2AngleIncrement( numberOfSamples );
    
    stringstream ss;
    ss<<"Validating SICK config: rangeres="<<rangeResolution<<" angleincrem="<<angleIncrement<<" baud="<<baudrate;
    context_.tracer()->debug( ss.str(), 4 );

    if ( !sickutil::isConfigValid( rangeResolution, angleIncrement, baudrate ) )
    {
        context_.tracer()->warning( "Initial configuration could not be implemented!  Check config file." );
        return 1;
    }

    //
    // now convert to Carmen format
    //

    // range resolution
    range_res_t range_res;
    if ( (int) rint(rangeResolution*1000) == 1 )
        range_res = MM;
    else if ( (int) rint(rangeResolution*1000) == 10 )
        range_res = CM;
    else 
        return -1;

    // Set the angular resolution
    double res_deg;
    if ( (int) rint(RAD2DEG(angleIncrement*1000)) == 1000 )
        res_deg = 1.0;
    else if ( (int) rint(RAD2DEG(angleIncrement*1000)) == 500 )
        res_deg = 0.5;
    else if ( (int) rint(RAD2DEG(angleIncrement*1000)) == 250 )
        res_deg = 0.25;
    else
        return -1;

    // Set the baud rate
    try {
        laser_settings_t laserSettings( device_, 
                                        type_,
                                        baudrate,
                                        res_deg,
                                        range_res );

        if ( laser_ ) delete laser_;
        //
        // Core object (re)sinitialisation
        //
        laser_ = new sick_laser_t( laserSettings );
    }
    catch ( std::string &e )
    {
        context_.tracer()->error( "Error setting params: " + e );
        return -1;
    }
    return 0;
}

void
SickCarmenDriver::init()
{
    firstRead_     = false;
    laserStalled_  = false;

    //
    // this talks to the laser and configures it
    //
    if ( setupParams( config_.maxRange, config_.numberOfSamples, baudrate_ ) )
    {
        throw orcaice::HardwareException( ERROR_INFO,
                                          "SickCarmenDriver::init(): Failed to setup factory config parameters." );
    }
 
    std::stringstream ss;
    ss << "Connecting to hardware with baudrate="<<baudrate_<<" device="<<device_<<" type="<<type_;
    context_.tracer()->info( ss.str() );

    //
    // tell the laser to start sending data
    //
    int ret = sick_start_laser(laser_);
    if ( ret == 0 )
    {
        context_.tracer()->info( "SickCarmenDriver::init(): connect succeeded." );
    }
    else
    {
        context_.tracer()->info( "SickCarmenDriver::init(): connect failed." );
        throw orcaice::HardwareException( ERROR_INFO,
                                          "SickCarmenDriver::init(): sick_start_laser failed" );
    }
}

void
SickCarmenDriver::read( std::vector<float> &ranges, 
                        std::vector<unsigned char> &intensities, 
                        orca::Time &timeStamp )
{
    context_.tracer()->debug( "SickCarmenDriver::read()", 4 );

    double currentTime;
    const  double LASER_STALL_TIMEOUT = 1.0; // seconds
    const  int    POLL_PERIOD_US = 1000;

    if ( laser_->timestamp == 0 )
    {
        // Haven't read anything yet
        laser_->timestamp = carmen_get_time_ms();
    }

    int i=0;
    std::string infoMessages = "";

    // Poll till we get new data, or the laser is screwed.
    while ( true )
    {
        if ( (i++ % 1000) == 0 )
            context_.tracer()->debug( "SickCarmenDriver: start of read while loop.", 5 );

        usleep(POLL_PERIOD_US);

        if ( firstRead_ ) {
            lastStatsUpdateTime_ = carmen_get_time_ms();
            firstRead_ = false;
        }
        currentTime = carmen_get_time_ms();

        sick_handle_laser(laser_);
        if ( strlen(sick_info()) != 0 ) {
            infoMessages = string("sick_handle_laser: %s") + sick_info();
        }

        laserStalled_ = ( currentTime - laser_->timestamp > LASER_STALL_TIMEOUT );
        if ( laserStalled_ )
        {
//             cout<<"TRACE(nativelaserdriver.cpp): returning on stall" << endl;
            throw hydroutil::Exception( ERROR_INFO, "SickCarmenDriver: laser stalled." );
        }

        if ( laser_->new_reading ) {
            // set the time stamp right away
            orcaice::setToNow( timeStamp );
            
            //debug
//            cout<<"SickCarmenDriver::read"<<endl;
            for ( int i=0; i < laser_->numvalues; i++ )
            {
                // alexm: dodgy hack in response to sudden shrinkage of the scan
                // "/1000.0" was in the original
                ranges[i]      = laser_->range[i]/100.0;
                //ranges[i]      = laser_->range[i]/1000.0;
                intensities[i] = laser_->glare[i];

                // debug
//                cout<<data->ranges[i]<<" ";
            }
//            cout<<endl;
            return;
        }
    }
}

// const std::string
// SickCarmenDriver::heartbeatMessage()
// {
//     sprintf(sickInfoMessage_, 
//             "Buffer: %s(%.1fpct full) ", 
//             laserStalled_ ? "STALLED " : " ", 
//             (laser_->buffer_position-laser_->processed_mark) / (float)LASER_BUFFER_SIZE * 100.0);
//     return sickInfoMessage_;
// }

} // namespace

extern "C" {
    laser2d::DriverFactory *createDriverFactory()
    { return new laser2d::SickCarmenDriverFactory; }
}
