/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Michael Moser
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#include "driver.h"

#include <hokuyo_aist/hokuyo_aist.h>

#include <gbxutilacfr/exceptions.h>
#include <gbxutilacfr/mathdefs.h>
#include <IceUtil/Time.h>

#include <termios.h>

#include <iostream>
#include <sstream>
#include <cstring>



using namespace std;

namespace laserscanner2dhokuyoaist {

Driver::Driver( const Config &cfg, const hydroutil::Context &context ) :
    config_(cfg),
    context_(context)
{
    laser_.reset( new hokuyo_aist::HokuyoLaser() );
    assert(0 != laser_.get());
    init();
}

Driver::~Driver()
{
    try{
        laser_->SetPower(false);
        laser_->Close();
    }
    catch(...){
        //no throwing from destructors
        context_.tracer().debug( "laser_->Close() is trying to throw something in the destructor, catch and ignore!", 2 );
    }
}

void
Driver::init()
{
    // read driver-specific properties
    std::string prefix = "HokuyoAist.";
    int baudrate =  context_.properties().getPropertyAsIntWithDefault( prefix+"Baudrate", 19200 );
    std::string dev = context_.properties().getPropertyWithDefault( prefix+"Device", "/dev/ttyACM0" );
    bool highSensitivity = context_.properties().getPropertyAsIntWithDefault( prefix+"HighSensitivity", 0 );

    context_.tracer().debug( "initializing driver",2 );
    try
    {
        stringstream ss;
        ss << "type=serial,device=" << dev << ",timeout=1,baud="<< baudrate;
        laser_->Open ( ss.str());
        laser_->SetPower(true);
        if(highSensitivity){
            laser_->SetHighSensitivity(true);
        }else{
            laser_->SetHighSensitivity(false);
        }
        try{
            laser_->SetBaud(baudrate);
        }
        catch (hokuyo_aist::HokuyoError &e){
            ss.str("");
            ss << "Failed to change baud rate: [" << e.Code () << "] " << e.what ()
                << " This can happen if the device is USB" << endl;
            context_.tracer().info( ss.str() );
        }

        hokuyo_aist::HokuyoSensorInfo info;
        laser_->GetSensorInfo (&info);
        ss.str("");
        ss << "Laser info:\n" << info.AsString ();
        context_.tracer().info( ss.str() );

        //check that hokuyo config matches our config
        if( NEAR(config_.maxRange, info.maxRange/1000.0, 1e-4)                  // ~1/10 mm
            && NEAR(config_.minRange, info.minRange/1000.0, 1e-4)               // ~1/10 mm
            && NEAR(config_.startAngle, info.minAngle, 1e-4)                    // ~1/200 deg
            && NEAR(config_.fieldOfView, info.maxAngle-info.minAngle, 1e-4)     // ~1/200 deg
            && config_.numberOfSamples == (int)info.scanableSteps
                ) {
            //all good
            context_.tracer().info("cfg matches!");
        }
        else{
            ss.str("");
            ss << "cfg mismatch!\n";
            ss <<  config_.maxRange << " ?? " << info.maxRange/1000.0 << endl
                << config_.minRange << " ?? " << info.minRange/1000.0 << endl
                << config_.startAngle << " ?? " << info.minAngle << endl
                << config_.fieldOfView << " ?? " << info.maxAngle-info.minAngle << endl
                << config_.numberOfSamples << " ?? " << info.scanableSteps << endl;
            throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
        }
    }
    catch (hokuyo_aist::HokuyoError &e)
    {
        std::stringstream ss;
        ss << "failed during initialisation of hokuyo_aist. Errorcode: "
            << e.Code()
            << ", Errordescription: "
            << e.what();
        throw gbxutilacfr::HardwareException( ERROR_INFO, ss.str() );
    }
    context_.tracer().debug( "driver initialized",2 );
}

void
Driver::read( hydrointerfaces::LaserScanner2d::Data &data )
{
    // tmp data storage
    hokuyo_aist::HokuyoData hokuyoData;

    try {
        laser_->GetRanges( &hokuyoData );
        // set the time stamp right away
        IceUtil::Time t = IceUtil::Time::now();
        data.timeStampSec = (int)t.toSeconds();
        data.timeStampUsec = (int)t.toMicroSeconds() - data.timeStampSec*1000000;

        //TODO: Michael 23/Sept/08
        //this timeStamp is actually at the _end_ of the scan; also
        //scan-time is _not_ negligable for the hokuyos (10-30Hz --> 100-33ms)
        //
        //--> need to keep track of delta time between scans, and use fieldOfView
        //to calculate a backcorrection, assuming 1 rotation per scan (true for
        //URG and UHG, unknown for TopURG). Also should put something like
        //durationOfScan into the interface.
        //
        //This would allow to interpolate senor pose per individual range-reading.
        //Yes, I really think this is neccessary.
    }
    catch (hokuyo_aist::HokuyoError &e)
    {
        std::stringstream ss;
        ss << "Read on urg_nz failed. Errorcode: "
            << e.Code()
            << ", Errordescription: "
            << e.what()
            << " continuing";
        //context_.tracer().error( ss.str() ,2 );
        throw gbxutilacfr::HardwareException( ERROR_INFO, ss.str() );
    }

    const uint32_t* ranges = hokuyoData.Ranges();
    //const uint32_t* intensities = hokuyoData.Intensities();
    size_t length = hokuyoData.Length();
    assert(config_.numberOfSamples == (int)length); // make sure the receiving end has enough space
    double minRangeHokuyo = config_.minRange*1000.0; // convert to mm
    for (unsigned int i=0; i<length; ++i )
    {
        if(ranges[i] < minRangeHokuyo){
            //these guys are error-codes (per reading); setting to maxRange to get behavior similar to SICK
            data.ranges[i]  = config_.maxRange;
        }else{
            data.ranges[i]  = ranges[i] / 1000.0;
        }
    }
}

} // namespace

extern "C" {
    hydrointerfaces::LaserScanner2dFactory *createDriverFactory()
    { return new laserscanner2dhokuyoaist::Factory; }
}
