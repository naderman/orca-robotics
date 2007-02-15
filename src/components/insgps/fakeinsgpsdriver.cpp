/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
 
#include <iostream>
#include <orcaice/orcaice.h>
#include <orcaobj/stringutils.h>

#include "fakeinsgpsdriver.h"

using namespace std;

namespace insgps{

FakeInsGpsDriver::FakeInsGpsDriver( const Config&           cfg,
                                    const orcaice::Context& context )
    : Driver(cfg, context),
      numReads_(0),
      context_(context)         
{
    // configure the buffers so they have size 100 and are of type queue
    gpsDataBuffer_.configure( 100 , orcaice::BufferTypeQueue );
    imuDataBuffer_.configure( 100 , orcaice::BufferTypeQueue );
    odometry3dDataBuffer_.configure( 100 , orcaice::BufferTypeQueue );
    localise3dDataBuffer_.configure( 100 , orcaice::BufferTypeQueue );
    
    context_.tracer()->info( "Initializing fake insgps driver with config: "+config_.toString() );
}

FakeInsGpsDriver::~FakeInsGpsDriver()
{
    // disable();
}

void
FakeInsGpsDriver::enable()
{   
    // isEnabled_=true;
//     start();
//     init();
    return;
}

int
FakeInsGpsDriver::init()
{
    context_.tracer()->info( "Initialising FakeInsGpsDriver... " );
    
    start();
    
    isEnabled_ = true;
    
    count_ = 0;
    
    // context_.tracer()->info( "FakeInsGpsDriver configuration has now been initialised to: "+config_.toString() );
    return 0;   
}               

void
FakeInsGpsDriver::run()
{     
    // We can't block in this loop -- have to keep it rolling so 
    // that isActive() is always checked.
    while ( isActive() )
    {
        try
        {
            readMsgsFromHardware();
            
        }
        catch ( FakeInsGpsException &e )
        {
            cout << "fakeinsgpsdriver::run(): Caught FakeInsGpsException: " << e.what() << endl;
        }
        catch ( ... )
        {
            cout << "fakeinsgpsdriver::run(): Caught some other exception..." << endl;
        }
    }
   
}               


int 
FakeInsGpsDriver::readMsgsFromHardware()
{
    context_.tracer()->debug("Generating fake insgps data",5);
   
    // imu data is sent at 100Hz
    orcaice::setToNow( imuData_.timeStamp );

    imuData_.accel.x = -35.0;
    imuData_.accel.y = 149.0;
    imuData_.accel.z = 50.0;
    imuData_.gyro.x  = 0.5;
    imuData_.gyro.y  = 1.0;
    imuData_.gyro.z  = 3.0;
    
    imuDataBuffer_.push( imuData_ );

    
    // only send gps data at 10Hz
    if ( count_ == 10)
    {
        // create fake gps data
        orcaice::setToNow( gpsData_.timeStamp );
        
        gpsData_.latitude = -35.0;
        gpsData_.longitude = 149.0;
        gpsData_.altitude = 50;
        gpsData_.heading = 20;
        gpsData_.speed = 5;
        gpsData_.climbRate = 3;
        gpsData_.satellites = 6;
        gpsData_.positionType = 1;
        gpsData_.geoidalSeparation = 10;

        gpsDataBuffer_.push( gpsData_ );

        // create fake pva data
        orcaice::setToNow( odometry3dData_.timeStamp );

        // position
        odometry3dData_.pose.p.x = -2 + ((numReads_%20)*0.2);
        odometry3dData_.pose.p.y = 2;
        odometry3dData_.pose.p.z = 3;

        // attitude
        odometry3dData_.pose.o.r = 4;
        odometry3dData_.pose.o.p = 5;
        odometry3dData_.pose.o.y = (-180 + numReads_*5 % 360)*M_PI/180.0;

        // velocity
        odometry3dData_.motion.v.x = 7;
        odometry3dData_.motion.v.y = 8;
        odometry3dData_.motion.v.z = 9;
        odometry3dData_.motion.w.x = 10;
        odometry3dData_.motion.w.y = 11;
        odometry3dData_.motion.w.z = 12;

        odometry3dDataBuffer_.push( odometry3dData_ );
        
        // create fake pva data
        orcaice::setToNow( localise3dData_.timeStamp );

        localise3dData_.hypotheses.resize(1);
        localise3dData_.hypotheses[0].weight = 1;
        // position
        localise3dData_.hypotheses[0].mean.p.x = -2 + ((numReads_%20)*0.2);
        localise3dData_.hypotheses[0].mean.p.y = 2;
        localise3dData_.hypotheses[0].mean.p.z = 3;

        // attitude
        localise3dData_.hypotheses[0].mean.o.r = 4;
        localise3dData_.hypotheses[0].mean.o.p = 5;
        localise3dData_.hypotheses[0].mean.o.y = (-180 + numReads_*5 % 360)*M_PI/180.0;

        // covariances
        localise3dData_.hypotheses[0].cov.xx = 1;
        localise3dData_.hypotheses[0].cov.xy = 0;
        localise3dData_.hypotheses[0].cov.xz = 0;
        localise3dData_.hypotheses[0].cov.yy = 1;
        localise3dData_.hypotheses[0].cov.yz = 0;
        localise3dData_.hypotheses[0].cov.zz = 1;

        localise3dData_.hypotheses[0].cov.xr = 0;
        localise3dData_.hypotheses[0].cov.xp = 0;
        localise3dData_.hypotheses[0].cov.xa = 0;
        localise3dData_.hypotheses[0].cov.yr = 0;
        localise3dData_.hypotheses[0].cov.yp = 0;
        localise3dData_.hypotheses[0].cov.ya = 0;
        localise3dData_.hypotheses[0].cov.zr = 0;
        localise3dData_.hypotheses[0].cov.zp = 0;
        localise3dData_.hypotheses[0].cov.za = 0;

        localise3dData_.hypotheses[0].cov.rr = 1;
        localise3dData_.hypotheses[0].cov.rp = 0;
        localise3dData_.hypotheses[0].cov.ra = 0;
        localise3dData_.hypotheses[0].cov.pp = 1;
        localise3dData_.hypotheses[0].cov.pa = 0;
        localise3dData_.hypotheses[0].cov.aa = 1;

        localise3dDataBuffer_.push( localise3dData_ );

        // reset the counter      
        count_ = 0;
    }
    
    count_++;   
    
    numReads_++;
    IceUtil::ThreadControl::sleep(IceUtil::Time::milliSeconds(10));
    return 0;
}

void
FakeInsGpsDriver::readGps(orca::GpsData& data, int timeoutMs )
{
    // blocking read with timeout. Also deletes the front element from the buffer
    int ret = gpsDataBuffer_.getAndPopNext( data, timeoutMs );
    if ( ret != 0 ) {
        throw FakeInsGpsException( "Timeout while waiting for GPS packet" );
    }
   
//     cout << "Gps Data Buffer has " << gpsDataBuffer_.size() << " elements" << endl;
    
    return;
    
}

void
FakeInsGpsDriver::readGpsTime(orca::GpsTimeData& data, int timeoutMs )
{
//     if(newGpsTime_)
//     {
//         newGpsTime_=false;
//         *data=gpsTimeData_;
//         return 0;
//     }
//     else
//     {
//         return -1;
//     }
return;
}

void
FakeInsGpsDriver::readImu(orca::ImuData& data, int timeoutMs )
{
    // blocking read with timeout. Also deletes the front element from the buffer
    int ret = imuDataBuffer_.getAndPopNext( data, timeoutMs );
    if ( ret != 0 ) {
        throw FakeInsGpsException( "Timeout while waiting for Imu packet" );
    }
   
//     cout << "Imu Data Buffer has " << imuDataBuffer_.size() << " elements" << endl;
    
    return;
}

void
FakeInsGpsDriver::readOdometry3d(orca::Odometry3dData& data, int timeoutMs )
{
    // blocking read with timeout. Also deletes the front element from the buffer
    int ret = odometry3dDataBuffer_.getAndPopNext( data, timeoutMs );
    if ( ret != 0 ) {
        throw FakeInsGpsException( "Timeout while waiting for Imu packet" );
    }
   
//     cout << "Odometry3d Data Buffer has " << odometry3dDataBuffer_.size() << " elements" << endl;
    
    return;
}

void
FakeInsGpsDriver::readLocalise3d(orca::Localise3dData& data, int timeoutMs )
{
    // blocking read with timeout. Also deletes the front element from the buffer
    int ret = localise3dDataBuffer_.getAndPopNext( data, timeoutMs );
    if ( ret != 0 ) {
        throw FakeInsGpsException( "Timeout while waiting for Imu packet" );
    }
   
    cout << "Localise3d Data Buffer has " << localise3dDataBuffer_.size() << " elements" << endl;
    
    return;
}


void
FakeInsGpsDriver::shutdown()
{
    // context_.tracer()->debug( "stopping fake insgps driver", 5 );
    // orcaice::Thread::stopAndJoin( this );
    // context_.tracer()->debug( "stopped fake insgps driver", 5 );
}               


} //namespace
