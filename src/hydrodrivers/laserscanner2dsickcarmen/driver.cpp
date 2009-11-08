/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <IceUtil/Time.h>
#include "sickutil.h"
#include <gbxutilacfr/mathdefs.h>
#include "driver.h"
#include "sick.h"
#include "carmen_conversion.h"
#include <gbxutilacfr/exceptions.h>

using namespace std;

namespace laserscanner2dsickcarmen {

Driver::Driver( const Config &cfg, const hydroutil::Context &context )
    : config_(cfg),
      laser_(0),
      context_(context)
{
    // read driver-specific properties
    std::string prefix = "SickCarmen.";

    baudrate_ =  context_.properties().getPropertyAsIntWithDefault( prefix+"Baudrate", 38400 );

    std::string device = context_.properties().getPropertyWithDefault( prefix+"Device", "/dev/ttyS0" );
    device_ = strdup(device.c_str());

    std::string laserType = context_.properties().getPropertyWithDefault( prefix+"LaserType", "LMS" );
    type_ = strdup(laserType.c_str());

    init();
}

Driver::~Driver()
{
    if ( laser_ ) delete laser_;
}

int
Driver::setupParams( double maxRange, int numberOfSamples, int baudrate )
{    
    //
    // first convert from generic configs to SICK-specific ones
    //

    double rangeResolution = sickutil::maxRange2RangeResolution( maxRange );
    double angleIncrement = sickutil::numberOfSamples2AngleIncrement( numberOfSamples );
    
    stringstream ss;
    ss<<"Validating SICK config: rangeres="<<rangeResolution<<" angleincrem="<<angleIncrement<<" baud="<<baudrate;
    context_.tracer().debug( ss.str(), 4 );

    if ( !sickutil::isConfigValid( rangeResolution, angleIncrement, baudrate ) )
    {
        context_.tracer().warning( "Initial configuration could not be implemented!  Check config file." );
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
        context_.tracer().error( "Error setting params: " + e );
        return -1;
    }
    return 0;
}

void
Driver::init()
{
    firstRead_     = false;
    laserStalled_  = false;

    //
    // this talks to the laser and configures it
    //
    if ( setupParams( config_.maxRange, config_.numberOfSamples, baudrate_ ) )
    {
        throw gbxutilacfr::HardwareException( ERROR_INFO,
                                              "Driver::init(): Failed to setup factory config parameters." );
    }
 
    std::stringstream ss;
    ss << "Connecting to hardware with baudrate="<<baudrate_<<" device="<<device_<<" type="<<type_;
    context_.tracer().info( ss.str() );

    //
    // tell the laser to start sending data
    //
    int ret = sick_start_laser(laser_);
    if ( ret == 0 )
    {
        context_.tracer().info( "Driver::init(): connect succeeded." );
    }
    else
    {
        context_.tracer().info( "Driver::init(): connect failed." );
        throw gbxutilacfr::HardwareException( ERROR_INFO,
                                          "Driver::init(): sick_start_laser failed" );
    }
}

void
Driver::read( hydrointerfaces::LaserScanner2d::Data &data )
{
    context_.tracer().debug( "Driver::read()", 4 );

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
            context_.tracer().debug( "Driver: start of read while loop.", 5 );

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
            throw gbxutilacfr::Exception( ERROR_INFO, "Driver: laser stalled." );
        }

        if ( laser_->new_reading ) {
            // set the time stamp right away
            IceUtil::Time t = IceUtil::Time::now();
            data.timeStampSec = (int)t.toSeconds();
            data.timeStampUsec = (int)t.toMicroSeconds() - data.timeStampSec*1000000;
            
            //debug
//            cout<<"Driver::read"<<endl;
            for ( int i=0; i < laser_->numvalues; i++ )
            {
                // alexm: dodgy hack in response to sudden shrinkage of the scan
                // "/1000.0" was in the original
                data.ranges[i]      = laser_->range[i]/100.0;
                //ranges[i]      = laser_->range[i]/1000.0;
                data.intensities[i] = laser_->glare[i];

                // debug
//                cout<<data->ranges[i]<<" ";
            }
//            cout<<endl;
            return;
        }
    }
}

// const std::string
// Driver::heartbeatMessage()
// {
//     sprintf(sickInfoMessage_, 
//             "Buffer: %s(%.1fpct full) ", 
//             laserStalled_ ? "STALLED " : " ", 
//             (laser_->buffer_position-laser_->processed_mark) / (float)LASER_BUFFER_SIZE * 100.0);
//     return sickInfoMessage_;
// }

} // namespace

extern "C" {
    hydrointerfaces::LaserScanner2dFactory *createLaserScanner2dDriverFactory()
    { return new laserscanner2dsickcarmen::Factory; }
}
