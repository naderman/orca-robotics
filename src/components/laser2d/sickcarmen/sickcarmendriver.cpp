/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#include <iostream>

#include "sickcarmendriver.h"

#include <sicklaserutil.h>
#include "carmen_conversion.h"
#include <orcaice/orcaice.h>

using namespace std;

namespace laser2d {

SickCarmenDriver::SickCarmenDriver( const char       *device,
                                    const char       *laserType,
                                    const int         baudrate,
                                    orcaice::Context  context )
    : enabled_(false),
      laser_(NULL),
      device_(strdup(device)),
      type_(strdup(laserType)),
      baudrate_(baudrate),
      context_(context)
{
    std::stringstream ss;
    ss << "Instantiating SickCarmenDriver at baud rate " << baudrate;
    context_.tracer()->print( ss.str() );

    if ( setupParams( DEG2RAD(1.0), 0.01, baudrate_ ) )
    {
        context_.tracer()->error( "Something went horribly wrong in constructor." );
        exit(1);
    }

    currentConfig_ = new orca::RangeScanner2dConfig;
    currentConfig_->isEnabled = false;
}

SickCarmenDriver::~SickCarmenDriver()
{
    if ( laser_ ) delete laser_;
}

int
SickCarmenDriver::setupParams( double angleIncrement, double rangeResolution, int baudrate )
{
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

    // Set the rangeResolution
    range_res_t range_res;
    if ( (int) rint(rangeResolution*1000) == 1 )
        range_res = MM;
    else if ( (int) rint(rangeResolution*1000) == 10 )
        range_res = CM;
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
        laser_ = new sick_laser_t( laserSettings );
    }
    catch ( std::string &e )
    {
        infoMessages_ += "Error setting params: " + e;
        return -1;
    }
    return 0;
}

int 
SickCarmenDriver::enable( )
{
    infoMessages_ = "";
    return doEnable();
}

int 
SickCarmenDriver::doEnable( )
{
    if ( enabled_ ) return 0;

    firstRead_     = false;
    laserStalled_  = false;
 
    int ret = sick_start_laser(laser_);
    if ( ret == 0 )
    {
        infoMessages_ += infoMessages_ + string("Successfully enabled laser:\n") + sick_info();
        enabled_ = true;
    }
    else
    {
        infoMessages_ += infoMessages_ + string("Problem enabling laser:\n") + sick_info();
    }
    return ret;
}

int 
SickCarmenDriver::disable()
{
    infoMessages_ = "";
    return doDisable();
}

int 
SickCarmenDriver::doDisable()
{
    if ( !enabled_ ) return 0;

    int ret = sick_stop_laser(laser_);
    if ( ret != 0 )
    {
        infoMessages_ += string("Problem stopping laser:\n") + sick_info();
    }
    infoMessages_ += string("Stopped laser:\n") + sick_info();

    enabled_ = false;
    return 0;
}

bool 
SickCarmenDriver::isEnabled()
{
    return enabled_;
}

int 
SickCarmenDriver::read( orca::LaserScanner2dDataPtr &data )
{
    infoMessages_ = "";
    assert( enabled_ );

//     cout<<"TRACE(nativelaserdriver.cpp): read()" << endl;

    double currentTime;
    const  double LASER_STALL_TIMEOUT = 1.0; // seconds
    const  int    POLL_PERIOD_US = 1000;

    // Poll till we get new data, or the laser is screwed.
    while ( true )
    {
//         cout<<"TRACE(nativelaserdriver.cpp): start of read while loop." << endl;

        usleep(POLL_PERIOD_US);

        if ( firstRead_ ) {
            lastStatsUpdateTime_ = carmen_get_time_ms();
            firstRead_ = 0;
        }
        currentTime = carmen_get_time_ms();

        sick_handle_laser(laser_);
        if ( strlen(sick_info()) != 0 ) {
            infoMessages_ = string("sick_handle_laser: %s") + sick_info();
        }

        if ( laser_->timestamp == 0 ) {
            // Haven't read anything yet.
            laserStalled_ = false;
        }
        else {
            laserStalled_ = ( currentTime - laser_->timestamp > LASER_STALL_TIMEOUT );
        }

        if ( laserStalled_ )
        {
//             cout<<"TRACE(nativelaserdriver.cpp): returning on stall" << endl;
            return -1;
        }

        if ( laser_->new_reading ) {
            // set the time stamp right away
            orcaice::setToNow( data->timeStamp );
            
            data->ranges.resize( laser_->numvalues );
            data->intensities.resize( laser_->numvalues );
            
            //debug
//            cout<<"SickCarmenDriver::read"<<endl;
            for ( int i=0; i < laser_->numvalues; i++ )
            {
                // alexm: dodgy hack in response to sudden shrinkage of the scan
                // "/1000.0" was in the original
                data->ranges[i]      = laser_->range[i]/100.0;
                //data->ranges[i]      = laser_->range[i]/1000.0;
                data->intensities[i] = laser_->glare[i];

                // debug
//                cout<<data->ranges[i]<<" ";
            }
//            cout<<endl;
            
            data->startAngle     = -M_PI/2;
            data->angleIncrement = DEG2RAD(((laser_->settings.angle_resolution==RES_1_00_DEGREE)?1.0:((laser_->settings.angle_resolution==RES_0_50_DEGREE)?0.5:((laser_->settings.angle_resolution==RES_0_25_DEGREE)?0.25:0.0))));
//             cout<<"TRACE(nativelaserdriver.cpp): read() ok." << endl;
            return 0;
        }
    }
}

const std::string
SickCarmenDriver::heartbeatMessage()
{
    sprintf(sickInfoMessage_, 
            "Buffer: %s(%.1fpct full) ", 
            laserStalled_ ? "STALLED " : " ", 
            (laser_->buffer_position-laser_->processed_mark) / (float)LASER_BUFFER_SIZE * 100.0);
    return sickInfoMessage_;
}

int 
SickCarmenDriver::setConfig( const orca::RangeScanner2dConfigPtr &cfg )
{
    infoMessages_ = "";

    // handle enable/disable
    if ( !cfg->isEnabled )
    {
        return disable();
    }

    // At this point, cfg->isEnabled must be set.  
    // Disable then re-enable with a new configuration.
//alexm todo:
//     if ( disable() ) {
//         return -1;
//     }
//     if ( setupParams( cfg->angleIncrement, cfg->rangeResolution, baudrate_ ) ) {
//         return -1;
//     }
    if ( cfg->isEnabled )
    {
        int ret = enable();
        if ( ret == 0 )
        {
            Ice::ObjectPtr super = cfg->ice_clone();
            currentConfig_ = orca::RangeScanner2dConfigPtr::dynamicCast( super );
        }
        return ret;
    }
    return 0;
}

int 
SickCarmenDriver::getConfig( orca::RangeScanner2dConfigPtr &cfg )
{
    cfg = currentConfig_;
    return 0;
}

}
