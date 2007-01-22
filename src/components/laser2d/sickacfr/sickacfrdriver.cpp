/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#include <iostream>
#include <orcaice/orcaice.h>
#include <sickutil.h>

#include "sickacfrdriver.h"
#include "laser.h"

using namespace std;

namespace laser2d {

SickAcfrDriver::SickAcfrDriver( const Config & cfg, const orcaice::Context & context )
    : Driver(cfg),
      laser_(0),
	  timeoutMs_(2000),
      laserCount_(0),
	  context_(context)
{
    // read driver-specific properties
    Ice::PropertiesPtr prop = context_.properties();
    std::string prefix = context_.tag()+".Config.SickAcfr.";

    baudrate_ =  orcaice::getPropertyAsIntWithDefault( prop, prefix+"Baudrate", 38400 );

    std::string device = orcaice::getPropertyWithDefault( prop, prefix+"Device", "/dev/ser1" );
    device_ = strdup(device.c_str());

//    std::string laserType = orcaice::getPropertyWithDefault( prop, prefix+"LaserType", "LMS" );
//    type_ = strdup(laserType.c_str());
}

SickAcfrDriver::~SickAcfrDriver()
{
    if ( laser_ ) delete laser_;
}


int
SickAcfrDriver::setupParams( double maxRange, int numberOfSamples, int baudrate )
{    
/*    //
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
    // now convert to Acfr format
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

    // Put the params into a sick laser structure and verify the params
    // Note: unlike the carmen driver, the baud rate is not set here
    try 
    {
       //  LaserSettings laserSettings( device_,
                                     type_,
                                     baudrate,
                                     res_deg,
                                     range_res );

        // if ( laser_ ) delete laser_;
        //
        // Core object (re)initialisation
        //
 */
        laser_ = new Laser();
/*    }
    catch ( std::string &e )
    {
        infoMessages_ += "Error setting params: " + e;
        return -1;
    }
*/
return 0;

}


int 
SickAcfrDriver::init( )
{
    // firstRead_     = false;
    // laserStalled_  = false;

    //
    // this talks to the laser and configures it
    //
    if ( setupParams( config_.maxRange, config_.numberOfSamples, baudrate_ ) )
    {
        context_.tracer()->error( "Failed to setup factory config parameters." );
        exit(1);
    }

 
//    std::stringstream ss;
//    ss << "Connecting to hardware with baudrate="<<baudrate_<<" device="<<device_<<" type="<<type_;
//    context_.tracer()->info( ss.str() );

    //
    // tell the laser to start sending data
    //
    // int ret = IniLaserInstance(1,9600,38400,1, 1);   //lsr2  , 38Kbps  , /dev/ser1);
    laser_->IniLaserInstance( 1, 9600, baudrate_, 1, device_ );   //lsr1  , 38Kbps  , /dev/ser1);

/*    if ( ret == 0 )
    {
        context_.tracer()->info( "connect succeeded." );
        infoMessages_ += infoMessages_ + string("Successfully enabled laser:\n") + sick_info();
    }
    else
    {
        context_.tracer()->info( "connect failed." );
        infoMessages_ += infoMessages_ + string("Problem enabling laser:\n") + sick_info();
    }
    return ret;
    */
    return 0;
}

int 
SickAcfrDriver::read( orca::LaserScanner2dDataPtr &data )
{
    infoMessages_ = "";

    context_.tracer()->debug( "SickAcfrDriver::read()", 4 );

    // double currentTime;
    // const  double LASER_STALL_TIMEOUT = 1.0; // seconds
    // const  int    POLL_PERIOD_US = 1000;
 
     // blocking read with timeout. Also deletes the front element from the buffer
    int ret = laser_->laserDataBuffer_.getAndPopNext( rawLaserData_, timeoutMs_ );
    if ( ret != 0 ) {
        // throw NovatelSpanException( "Timeout while waiting for GPS packet" );
        context_.tracer()->info( "TRACE(sickacfrdriver.cpp::read()): Timeout while waiting for laser packet" );
    }
    else
    {      
        context_.tracer()->debug( "TRACE(sickacfrdriver.cpp::read()): got laser data", 6 );

        if (laserCount_ > 100 )
        {
            cout << "Laser Data Buffer is " << laser_->laserDataBuffer_.size()/100 << "% full" << endl;
             laserCount_ = 0;
        }
        laserCount_++;
    
 		// TODO: put in lib/orcaobh/timeutils.h/cpp
    	//convert to microseconds
		// int usec = rawLaserData_.sp->timeStamp*10;
    	// cout << "usec: " << usec << endl;
		// convert to orca::Time
    	// data->timeStamp.seconds = (int)( floor( (double)usec/1000000 ) );
		// data->timeStamp.useconds = ( usec - (  data->timeStamp.seconds * 1000000 ) );
        data->timeStamp = rawLaserData_.sp->timeStamp;

		// TODO: change the number of scans so that it is configurable
	    data->ranges.resize( 361 );
		data->intensities.resize( 361 );
					            
		for ( int i=0; i < 361; i++ )
		{
		    // mask off the corresponding bytes in the sicklaser structure
			// last 3 bits are intensity
			data->intensities[i] = (rawLaserData_.sp->range[i]>>13) & 0x7;
 	  		// first 13 bits are range
			data->ranges[i] = rawLaserData_.sp->range[i] & 0x1FFF;
 			// convert from cm to metres
			data->ranges[i] = data->ranges[i]/100;

			//default settings
            data->minRange     = config_.minRange;
            data->maxRange     = config_.maxRange;
            data->fieldOfView  = config_.fieldOfView;
            data->startAngle   = config_.startAngle;
       }
	
    }

   return 0;
}


const std::string
SickAcfrDriver::heartbeatMessage()
{
    sprintf(sickInfoMessage_, "hello"); 
            //"Buffer: %s(%.1fpct full) "
           // laserStalled_ ? "STALLED " : " ", 
           // (laser_->buffer_position-laser_->processed_mark) / (float)LASER_BUFFER_SIZE * 100.0);
    return sickInfoMessage_;
}

} // namespace
