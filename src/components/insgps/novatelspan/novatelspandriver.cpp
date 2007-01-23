/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Matthew Ridley, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <orcaice/orcaice.h>
#include "novatelspandriver.h"
#include "latlon2mga.h"
#include "timeval.h"

// log types
#include <novatel/header.h>
#include <novatel/logtypes.h>
#include <novatel/crc/ncrc32.h>

#include <iostream>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

// enumerated BufferType's
#include <orcaice/common.h>

using namespace std;
using namespace orca;
using namespace orcaserial;

namespace insgps{

NovatelSpanInsGpsDriver::NovatelSpanInsGpsDriver( const char*             device,
                                                  int                     baud,
                                                  const Config&           cfg,
                                                  const orcaice::Context& context )
    : Driver(cfg, context),
      serial_(0),
      enabled_( false ),
      gpsCount_(0),
      imuCount_(0),
      odometry3dCount_(0),
      //      gpsData_(0),
//      imuData_(0),
//      odometry3dData_(0),
      context_(context)
{
    serial_ = new Serial();
    
    // TODO: stick this in an openPort function like the AtmelDriver
    if(serial_->open(device, O_NONBLOCK ) ==-1){
	cout << "NovatelSpanInsGps: ERROR: Failed to open serial device: " << device << endl;
        std::string errString = "Failed to open serial device.";
        throw orcaice::Exception( ERROR_INFO, errString );
    }
    if(serial_->baud(baud)==-1){
	cout << "NovatelSpanInsGps: ERROR: Failed to set baud rate.\n";
        std::string errString = "Failed to set baud rate.";
        throw orcaice::Exception( ERROR_INFO, errString );
    }
    // set to 1 second timeout
    serial_->timeout( 1,0 );
    // hasFix_       = false;
    // havePps_      = false;
    // havePosition_ = false;
    // havePva_      = false;

    // configure the buffers so they have depth 100 and are of type queue
    gpsDataBuffer_.configure( 100 , orcaice::BufferTypeQueue );
    imuDataBuffer_.configure( 100 , orcaice::BufferTypeQueue );
    odometry3dDataBuffer_.configure( 100 , orcaice::BufferTypeQueue );
   
}

NovatelSpanInsGpsDriver::~NovatelSpanInsGpsDriver()
{
    disable();
    // context_.tracer()->debug( "TRACE(novatelspandriver::~novatelspandriver()): NovatelSpan driver disabled", 5 );
    if(serial_!=NULL)
	delete serial_;
    // context_.tracer()->debug( "TRACE(novatelspandriver::~novatelspandriver()): serial_ pointer deleted", 5 );
}

int
NovatelSpanInsGpsDriver::reset()
{
    cout << "NovatelSpanInsGps: resetting Novatel Span InsGps driver\n";

//     serial_->flush();
//     const char *reset_string = "$PASHS,RST\r\n";
//     serial_->write(reset_string, strlen(reset_string));
//     char sdata[256];
//     int ret;
//     sleep(2);
// 
//     bool reset_ok=false;
// 
//     // do this multiple times just in case it is already spitting out data
//     do{
//         ret=serial_->read_line(sdata,256,'\n');
// 
//         if(ret>0){
//             // stick a terminating char in to print
//             //sdata[ret]=0;
//             //printf("got %d bytes:%s\n",ret,sdata);
// 	    if(strncmp(sdata,"$PASHR,ACK*3D",13)==0){
//                 reset_ok=true;
//             }
//         }
//     }while(ret>0);
// 
//     if(reset_ok==false){
// 	cout << "AshtechGps: ERROR: AshTech GPS failed to reset\n";
//         cout << "AshtechGps: Disabling Ashtech GPS driver\n";
//         enabled_ = false;
//         return -1;
//     }
// 
//     // but of course we still have to wait a little longer grrrr....
//     sleep(2);
// 
//     cout << "AshtechGps: AshTech GPS reset sucessfully\n";
    return 0;
}

void
NovatelSpanInsGpsDriver::enable()
{
    // start the orcaice::thread for this driver 
    start();
    
    if ( init() < 0 )
    {
        cout << "ERROR(novatelspandriver.cpp): Couldn't initialise Novatel serial driver" << endl;
        enabled_ = false;
    }               
    else
    {      
        enabled_ = true;
    }      
}

int
NovatelSpanInsGpsDriver::init()
{
    // start the orcaice::thread for this driver
    // start();


    if ( enabled_ )
        return 0;
    
    cout << "NovatelSpanInsGps: Initialising Novatel Span InsGps driver\n";

    int put;
    char response[13];
    char trash[256];
    std::string responseString;
    std::string responseOk;
    responseOk = "\r\n<OK";
	int count = 0;

    //
    // send initialisation commands to the Novatel   
    //
    
    while ( responseString != responseOk )
    {
      // just in case something is running... stops the novatel logging any messages
      serial_->flush();
      put = serial_->write( "unlogall\r\n" );
      //printf("put %d bytes\n",put);
      serial_->drain();
       
      // Read the serial device to check response ( "<OK" is 3 bytes long )
      // Note that the response is in abbreviated ASCII format so only need to check for "<OK"
        if ( serial_->read_full( response, 13 ) < 0 )
        {
            cout << "ERROR(novatelspandriver.cpp): Error reading from serial device" << endl;
            return -1;
        }
        else
        {
            responseString = response;
            responseString.resize(5);
        	cout << "responseString: " << responseString << endl;
		}

        if ( responseString != responseOk )
        {
            cout << "WARNING(novatelspandriver.cpp): Response to 'unlogall' returned error " << endl;
            cout << "\t We will continue trying to stop all messages so that we can reset the device" << endl;
            
            // check how much data is left in the input buffer and throw it away
            int unreadBytes = serial_->data_avail_wait();
            // cout << "unreadBytes: " << unreadBytes << endl;
            if (unreadBytes < 256 )
            {            
                serial_->read_full( trash, unreadBytes );

            }

            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(1));
        }
        else
        {
            cout << "INFO: response was ok: " << responseString << endl;
        }

        count++;
		if ( count > 3 )
		{
		     cout << "ERROR(novatelspandriver::init()):Could not initialise driver" << endl;
		exit(1);
		}
			
    } // end of while
        
    // tell the novatel what serial port the imu is attached to (com3 = aux)
    put = serial_->write( "interfacemode com3 imu imu on\r\n" );

     // Read the serial device to check response ( "<OK" is 3 bytes long )
    // Note that the response is in abbreviated ASCII format so only need to check for "<OK"   
    if ( serial_->read_full( response, 13 ) < 0 )
    {
        cout << "ERROR(novatelspandriver.cpp): Error reading from serial device" << endl;
    //     return -1;
    }
    else
    {
        responseString = response;
        responseString.resize(5);
    }

    if ( responseString != responseOk )
    {
        cout << "WARNING(novatelspandriver.cpp): Response to 'interfacemode' returned error: " << responseString << endl;
    //     return -1;
    }
   
    
    // for dgps
    // put = serial_->write( "com com2 57600 n 8 1 n off on\r\n" ); // My Addition
    // serial_->drain();
    // put = serial_->write( "interfacemode com2 rtca none off\r\n" ); // My Addition
    // serial_->drain();


    // turn off posave as thi command implements position averaging for base stations.
    put = serial_->write( "posave off\r\n" );
    
      // Read the serial device to check response ( "<OK" is 3 bytes long )
    // Note that the response is in abbreviated ASCII format so only need to check for "<OK"   
    if ( serial_->read_full( response, 13 ) < 0 )
    {
        cout << "ERROR(novatelspandriver.cpp): Error reading from serial device" << endl;
    //     return -1;
    }
    else
    {
        responseString = response;
        responseString.resize(5);
    }

    if ( responseString != responseOk )
    {
        cout << "WARNING(novatelspandriver.cpp): Response to 'posave' returned error: " << responseString << endl;
    //     return -1;
    }
   


    // make sure that fixposition has not been set
    put = serial_->write( "fix none\r\n" );
    
     // Read the serial device to check response ( "<OK" is 3 bytes long )
    // Note that the response is in abbreviated ASCII format so only need to check for "<OK"   
    if ( serial_->read_full( response, 13 ) < 0 )
    {
        cout << "ERROR(novatelspandriver.cpp): Error reading from serial device" << endl;
    //     return -1;
    }
    else
    {
        responseString = response;
        responseString.resize(5);
    }

    if ( responseString != responseOk )
    {
        cout << "WARNING(novatelspandriver.cpp): Response to 'fix' returned error: " << responseString << endl;
    //     return -1;
    }

    // imu/gps antenna offset
    getImuAntennaOffsetProperties();   
    char str[256];
    sprintf( str,"setimutoantoffset %f %f %f %f %f %f\r\n",
            imuAntennaOffset_.x, imuAntennaOffset_.y, imuAntennaOffset_.z, imuAntennaOffsetUncertainty_.x, imuAntennaOffsetUncertainty_.y, imuAntennaOffsetUncertainty_.z );
        //put = serial_->write("setimutoantoffset -0.04 -0.155 0.225 0.005 0.005 0.005\r\n");
    put = serial_->write( str );
    // serial_->drain();

 
    // the type of imu being used
    put = serial_->write( "setimutype imu_hg1700_ag17\r\n" );

     // Read the serial device to check response ( "<OK" is 3 bytes long )
    // Note that the response is in abbreviated ASCII format so only need to check for "<OK"   
    if ( serial_->read_full( response, 13 ) < 0 )
    {
        cout << "ERROR(novatelspandriver.cpp): Error reading from serial device" << endl;
    //     return -1;
    }
    else
    {
        responseString = response;
        responseString.resize(5);
    }

    if ( responseString != responseOk )
    {
        cout << "WARNING(novatelspandriver.cpp): Response to 'setimutype' returned error: " << responseString << endl;
    //     return -1;
    }

    // select the geodetic datum for operation of the receiver (wgs84 = default)
    put = serial_->write( "datum wgs84\r\n" );

     // Read the serial device to check response ( "<OK" is 3 bytes long )
    // Note that the response is in abbreviated ASCII format so only need to check for "<OK"   
    if ( serial_->read_full( response, 13 ) < 0 )
    {
        cout << "ERROR(novatelspandriver.cpp): Error reading from serial device" << endl;
    //     return -1;
    }
    else
    {
        responseString = response;
        responseString.resize(5);
    }

    if ( responseString != responseOk )
    {
        cout << "WARNING(novatelspandriver.cpp): Response to 'datum' returned error: " << responseString << endl;
    //     return -1;
    }
   
    // This command provides a method for controlling the polarity and rate of the PPS output
    // put = serial_->write( "ppscontrol enable negative 1.0\r\n" );
    // serial_->drain();

    // receiver status
    put = serial_->write( "log rxstatusb ontime 1.0\r\n" );
    
//      // Read the serial device to check response ( "<OK" is 3 bytes long )
//     // Note that the response is in abbreviated ASCII format so only need to check for "<OK"   
//     if ( serial_->read_full( response, 13 ) < 0 )
//     {
//         cout << "ERROR(novatelspandriver.cpp): Error reading from serial device" << endl;
//         return -1;
//     }
//     else
//     {
//         responseString = response;
//         responseString.resize(5);
//     }
// 
//     if ( responseString != responseOk )
//     {
//         cout << "WARNING(novatelspandriver.cpp): Response to 'setimutype' returned error: " << responseString << endl;
//         return -1;
//     }
    
    // PPS pulse will be triggered before this arrives
    
    // time used to sync with the pps signal
    put = serial_->write( "log timeb ontime 1.0\r\n" );
//      put = serial_->write("log timesyncb ontime 1.0\r\n");
        
//          // Read the serial device to check response ( "<OK" is 3 bytes long )
//     // Note that the response is in abbreviated ASCII format so only need to check for "<OK"   
//     if ( serial_->read_full( response, 13 ) < 0 )
//     {
//         cout << "ERROR(novatelspandriver.cpp): Error reading from serial device" << endl;
//         return -1;
//     }
//     else
//     {
//         responseString = response;
//         responseString.resize(5);
//     }
// 
//     if ( responseString != responseOk )
//     {
//         cout << "WARNING(novatelspandriver.cpp): Response to 'log timeb' returned error: " << responseString << endl;
//         return -1;
//     }
    
    // IMU message
    
    // gps position without ins
    // put = serial_->write( "log bestgpsposb ontime 1.0\r\n" );
    put = serial_->write( "log bestgpsposb ontime 0.05\r\n" );
        
//          // Read the serial device to check response ( "<OK" is 3 bytes long )
//     // Note that the response is in abbreviated ASCII format so only need to check for "<OK"   
//     if ( serial_->read_full( response, 13 ) < 0 )
//     {
//         cout << "ERROR(novatelspandriver.cpp): Error reading from serial device" << endl;
//         return -1;
//     }
//     else
//     {
//         responseString = response;
//         responseString.resize(5);
//     }
// 
//     if ( responseString != responseOk )
//     {
//         cout << "WARNING(novatelspandriver.cpp): Response to 'log bestgpsposb' returned error: " << responseString << endl;
//         return -1;
//     }

    //short IMU messages
    // pva data in wgs84 coordinates
    put = serial_->write( "log inspvasb ontime 0.01\r\n" );
    
//      // Read the serial device to check response ( "<OK" is 3 bytes long )
//     // Note that the response is in abbreviated ASCII format so only need to check for "<OK"   
//     if ( serial_->read_full( response, 13 ) < 0 )
//     {
//         cout << "ERROR(novatelspandriver.cpp): Error reading from serial device" << endl;
//         return -1;
//     }
//     else
//     {
//         responseString = response;
//         responseString.resize(5);
//     }
// 
//     if ( responseString != responseOk )
//     {
//         cout << "WARNING(novatelspandriver.cpp): Response to 'log inspvasb' returned error: " << responseString << endl;
//         return -1;
//     }
    
    // raw accelerometer and gyro data
    put = serial_->write( "log rawimusb onnew\r\n" );
    
//          // Read the serial device to check response ( "<OK" is 3 bytes long )
//     // Note that the response is in abbreviated ASCII format so only need to check for "<OK"   
//     if ( serial_->read_full( response, 13 ) < 0 )
//     {
//         cout << "ERROR(novatelspandriver.cpp): Error reading from serial device" << endl;
//         return -1;
//     }
//     else
//     {
//         responseString = response;
//         responseString.resize(5);
//     }
// 
//     if ( responseString != responseOk )
//     {
//         cout << "WARNING(novatelspandriver.cpp): Response to 'log rawimusb' returned error: " << responseString << endl;
//         return -1;
//     }

    // pva covariances
    put = serial_->write( "log inscovsb onchanged\r\n" );
     
//     // Read the serial device to check response ( "<OK" is 3 bytes long )
//     // Note that the response is in abbreviated ASCII format so only need to check for "<OK"   
//     if ( serial_->read_full( response, 13 ) < 0 )
//     {
//         cout << "ERROR(novatelspandriver.cpp): Error reading from serial device" << endl;
//         return -1;
//     }
//     else
//     {
//         responseString = response;
//         responseString.resize(5);
//     }
// 
//     if ( responseString != responseOk )
//     {
//         cout << "WARNING(novatelspandriver.cpp): Response to 'log inscovsb' returned error: " << responseString << endl;
//         return -1;
//     }

    start();
    enabled_ = true;

    return 0;
}

int
NovatelSpanInsGpsDriver::disable()
{
//    cout << "enabled_: " << enabled_ << endl;

    if ( !enabled_ ) return 0;

    cout << "NovatelSpanInsGps: Disabling Novatel Span InsGps driver\n";
 
    // int put;
    // put = serial_->write( "unlogall\r\n" );
    int put;
    char response[13];
    char trash[256];
    std::string responseString;
    std::string responseOk;
    responseOk = "\r\n<OK";

    //
    // send initialisation commands to the Novatel   
    //
    
    while ( responseString != responseOk )
    {
        // just in case something is running... stops the novatel logging any messages
        serial_->flush();
        put = serial_->write( "unlogall\r\n" );
        //printf("put %d bytes\n",put);
        serial_->drain();
        
        // Read the serial device to check response ( "<OK" is 3 bytes long )
        // Note that the response is in abbreviated ASCII format so only need to check for "<OK"
        if ( serial_->read_full( response, 13 ) < 0 )
        {
            cout << "ERROR(novatelspandriver.cpp): Error reading from serial device" << endl;
            return -1;
        }
        else
        {
            responseString = response;
            responseString.resize(5);
        }

        if ( responseString != responseOk )
        {
            cout << "WARNING(novatelspandriver.cpp): Response to 'unlogall' returned error " << endl;
            cout << "\t We will continue trying to stop all messages so that we can reset the device" << endl;
            
            // check how much data is left in the input buffer and throw it away
            int unreadBytes = serial_->data_avail_wait();
            // cout << "unreadBytes: " << unreadBytes << endl;
            if (unreadBytes < 256 )
            {
                serial_->read_full( trash, unreadBytes );

            }

            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(1));
        }
        else
        {
            cout << "INFO: all messages were stopped " << endl;
        }
    } // end of while     

    //force data to be sent
    // serial_->drain();

    // serial_->flush();
    
    enabled_ = false;

    return 0;
}

void
NovatelSpanInsGpsDriver::readGps( orca::GpsData& data, int timeoutMs )
{
    
    // blocking read with timeout. Also deletes the front element from the buffer
    int ret = gpsDataBuffer_.getAndPopNext( data, timeoutMs );
    if ( ret != 0 ) {
        // throw NovatelSpanException( "Timeout while waiting for GPS packet" );
        context_.tracer()->info( "TRACE(novatelspandriver::readGps()): Timeout while waiting for GPS packet" );
    }
    else
    {      
        context_.tracer()->debug( "TRACE(novatelspandriver::readGps()): got gps data", 6 );

        // cout << "gpsCount_: " << gpsCount_ << endl;
        if (gpsCount_ > 200 )
        {
            cout << "Gps Data Buffer is " << gpsDataBuffer_.size()/100 << "% full" << endl;
            gpsCount_ = 0;
        }
        gpsCount_++;
    
    }
    
    return;
}

void
NovatelSpanInsGpsDriver::readGpsTime( orca::GpsTimeData& data, int timeoutMs )
{
    return;   
}

void
NovatelSpanInsGpsDriver::readImu( orca::ImuData& data, int timeoutMs )
{
    // blocking read with timeout. Also deletes the front element from the buffer
    int ret = imuDataBuffer_.getAndPopNext( data, timeoutMs );
    if ( ret != 0 ) {
//        throw NovatelSpanException( "Timeout while waiting for IMU packet" );
        context_.tracer()->info( "TRACE(novatelspandriver::readImu()): Timeout while waiting for IMU packet" );
    }
    else
    {
        context_.tracer()->debug( "TRACE(novatelspandriver::readImu()): got imu data", 6 );
    
        if (imuCount_ > 1000 )
        {
            cout << "Imu Data Buffer is " << imuDataBuffer_.size()/100 << "% full" << endl;
            imuCount_ = 0;
        }
        imuCount_++;
    
    }
    
    return;
}
    
void
NovatelSpanInsGpsDriver::readOdometry3d( orca::Odometry3dData& data, int timeoutMs )
{
    // blocking read with timeout. Also deletes the front element from the buffer
    int ret = odometry3dDataBuffer_.getAndPopNext( data, timeoutMs );
    if ( ret != 0 ) {
        // throw NovatelSpanException( "Timeout while waiting for Odometry3d packet" );
        context_.tracer()->info( "TRACE(novatelspandriver::readOdometry3d()): Timeout while waiting for Odometry3d packet" );
    }
    else
    {   
        context_.tracer()->debug( "TRACE(novatelspandriver::readOdometry3d()): got odometry3d data", 6 );
        
        if (odometry3dCount_ > 200 )
        {
            cout << "Odometry3d Data Buffer is " << odometry3dDataBuffer_.size()/100 << "% full" << endl;
            odometry3dCount_ = 0;
        }
        odometry3dCount_++;
    
    }
    
    return;
}

void
NovatelSpanInsGpsDriver::run()
{
    try
    {    
        // We can't block in this loop -- have to keep it rolling so
        // that isActive() is always checked.
        while ( isActive() )
        {
            try
            {
                // Guaranteed not to block for long.
                int ret = serial_->data_avail_wait();
                if ( ret < 0 )
                    throw( std::string("Error waiting for data: ")+strerror(errno) );
                else if ( ret > 0 )
                    readMsgsFromHardware();
            }
            catch ( NovatelSpanException &e )
            {
                cout << "Caught NovatelSpanException: " << e.what() << endl;
            }
            catch ( orcaice::Exception & e )
            {
                std::stringstream ss;
                ss << "novatelspandriver::run(): Caught orcaice::exception: " << e.what();
                context_.tracer()->warning( ss.str() );
            }
            catch ( Ice::Exception & e )
            {
                std::stringstream ss;
                ss << "novatelspandriver::run(): Caught Ice::exception: " << e;
                context_.tracer()->warning( ss.str() );
            }
        
            catch ( ... )
            {
                cout << "novatelspandriver::run(): Caught some other exception..." << endl;
            }
        } // end of while
   
    } // end of try
    catch ( Ice::CommunicatorDestroyedException &e )
    {
        // This is OK: it means that the communicator shut down (eg via Ctrl-C)
        // somewhere in mainLoop.
    }
    catch ( Ice::Exception &e )
    {
        std::stringstream ss;
        ss << "ERROR(novatelspandriver::run()): Caught unexpected Ice exception: " << e;
        context_.tracer()->error( ss.str() );
    }
    catch ( std::exception &e )
    {
        std::stringstream ss;
        ss << "ERROR(novatelspandriver::run()): Caught unexpected std::exception: ";
        context_.tracer()->error( ss.str() );
    }
    catch ( ... )
    {
        std::stringstream ss;
        ss << "ERROR(novatelspandriver::run()): Caught unexpected unknown exception.";
        context_.tracer()->error( ss.str() );
    }
      
}               


int
NovatelSpanInsGpsDriver::readMsgsFromHardware()
{
    if ( ! enabled_ )
    {
	cout << "NovatelSpanInsGps: ERROR: Can't read: not enabled . Sleeping for 1 sec..." << endl;
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(1));
        return -1;
    }

    int ret;
    int msgs = 0;
        
    // read novatel binary messages
    ret = read_message( &serial_data_ );
	// ret = serial_->read_line(serial_data_,1024,'\n');

    timeOfRead_ = IceUtil::Time::now();

	if( ret>0 )
    {
        if( populateData( ret ) == 0 )
        {
		    msgs++;
		}
    }

    if ( ret <= 0 )
    {
        if( errno != EAGAIN )
        {
	        cout << "NovatelSpanInsGps: ERROR: Error reading from InsGps:" << strerror(errno) << " -- shutting down." << endl;
            enabled_ = false;
            return -1;
        }
    }
    
    //return how many messages read
    return msgs;
}


int
NovatelSpanInsGpsDriver::populateData( int id )
{    
    switch(id)
    {
        case novatel::RXSTATUSB_LOG_TYPE:
        {
            memcpy( &RXSTATUS_, &serial_data_.raw_message, sizeof(RXSTATUS_) );
            // printf("got RXSTATUS\n");
            return 0;       
            break;
        }
        // This case is used for dgps synchronisation which is not needed at the moment
        // TODO: add mkutctime in if needed                      
//         case novatel::TIMESYNCB_LOG_TYPE:
//         {
//             memcpy( &TIMESYNC_, &serial_data_.raw_message, sizeof(TIMESYNC_) );
// 
//             printf( "timesync week = %lu, mseconds = %lu\n",
//                    TIMESYNC_.data.week,
//                    TIMESYNC_.data.mseconds );
//             
//             struct timeval blah;
//             mkutctime(TIMESYNC_.data.week,
//                             ((double)TIMESYNC_.data.mseconds)/1000,
//                             &blah);
//             printf("timesync says it's %lu.%06lu\n",blah.tv_sec,blah.tv_usec);
// 
//             break;
//         }
        case novatel::TIMEB_LOG_TYPE:
        {
            // printf("got TIME\n");
            memcpy( &TIME_, &serial_data_.raw_message, sizeof(TIME_ ) );
            if( !TIME_.data.bUtcStatus )
            {
                printf("UTC time not available\n");
                break;
            }
            
            // retrieve all the offsets
            pps_.offset=TIME_.data.dGPSOffset;
            pps_.offset_std=TIME_.data.dOffsetStd;
            pps_.utc_offset=TIME_.data.dUtcOffset;

            gpsTimeData_.utcTime.seconds = TIME_.data.lUtcMillisec/1000;
            gpsTimeData_.utcTime.minutes = TIME_.data.ucUtcMin;
            gpsTimeData_.utcTime.hours = TIME_.data.ucUtcHour;
            gpsTimeData_.utcDate.day = TIME_.data.ucUtcDay;
            gpsTimeData_.utcDate.month = TIME_.data.ucUtcMonth-1;
            gpsTimeData_.utcDate.year = TIME_.data.lUtcYear-1900;
            
            // calculate UTC time
            // struct tm broken_time;
            // broken_time.tm_sec=TIME_.data.lUtcMillisec/1000;
            // broken_time.tm_min=TIME_.data.ucUtcMin;
            // broken_time.tm_hour=TIME_.data.ucUtcHour;
            // broken_time.tm_mday=TIME_.data.ucUtcDay;
            // broken_time.tm_mon=TIME_.data.ucUtcMonth-1;
            // broken_time.tm_year=TIME_.data.lUtcYear-1900;
            // do we have daylight savings at the moment?
            //broken_time.tm_isdst=-1;

//             // convert from broken down time to calendar time (see "man mktime")
//             struct timeval now;
//             now.tv_sec = mktime(&broken_time);
//             if( now.tv_sec == -1 )
//             {
//                 fprintf(stderr,"mktime() failed\n");
//             }
//             
//             // adjust for timezone
//             now.tv_sec- = timezone;
//             now.tv_usec=1000*(TIME_.data.lUtcMillisec%1000);
// 
//             // copy in
//             pps_.time=now;

            // set flag
            // newGpsTime_ = true;       
            
            return 0;       
            break;
        }
        case novatel::BESTGPSPOSB_LOG_TYPE:
        {
            // printf("got BESTGPSPOS\n");
            memcpy( &BESTGPSPOS_, &serial_data_.raw_message, sizeof(BESTGPSPOS_) );

            gpsData_.positionType = BESTGPSPOS_.data.pos_type;
            if( gpsData_.positionType==0 )
            {
                // newGpsData_ = false;
                cout << "TRACE(novatelspandriver.cpp): Position not yet available" << endl;
                return -1;
            }

            //Set time
            // gettimeofday(&position_.time,NULL);
            gpsData_.timeStamp = orcaice::toOrcaTime (timeOfRead_);

            // TODO: made up the zone here... need to read the real zone         
            int zone = 0;

            // double lat = BESTGPSPOS_.data.latitude;
            gpsData_.latitude = BESTGPSPOS_.data.latitude;
            // double lng = BESTGPSPOS_.data.longitude;
            gpsData_.longitude = BESTGPSPOS_.data.longitude;
            // position_.down = -BESTGPSPOS_.data.height;
            gpsData_.altitude = BESTGPSPOS_.data.height;
                
            // depends on the number of satellites... not sure how to include this in
            // the gps interface       
            sol_status_ = BESTGPSPOS_.data.sol_status;
            
            //LatLon2MGA(lat,lng,
            //           position_.northing, position_.easting, zone);
            
            position_.zone = zone;
            
            // set flag
            // newGpsData_ = true;

            // context_.tracer()->debug( "TRACE(novatelspandriver::populateData()): Pushing gps data to buffer", 5);
            gpsDataBuffer_.push( gpsData_ );
            // context_.tracer()->debug( "TRACE(novatelspandriver::populateData()): Pushed gps data to buffer", 5);
            
            return 0;       
            break;
        }
        case novatel::INSPVASB_LOG_TYPE:
        {
            // printf("got INSPVASB\n");
            memcpy( &INSPVA_, &serial_data_.raw_message, sizeof(INSPVA_) );

            odometry3dData_.timeStamp = orcaice::toOrcaTime (timeOfRead_);

            // cout << "lattitude and longitude: " << INSPVA_.data.latitude << " " << INSPVA_.data.longitude << endl;
            
            int zone;
            LatLon2MGA(INSPVA_.data.latitude, INSPVA_.data.longitude,
                       odometry3dData_.pose.p.x, odometry3dData_.pose.p.y, zone);

            // cout << "MGA x and y: " << odometry3dData_.pose.p.x << " " << odometry3dData_.pose.p.y << endl;


            // TODO: do we have to convert pva data into local coordinate frame? 
            // load the pva data into the odometry3d object       
            // odometry3dData_.pose.p.x = INSPVA_.data.latitude;
            // odometry3dData_.pose.p.y = INSPVA_.data.longitude;
            odometry3dData_.pose.p.z = -INSPVA_.data.height;
       
            //velocities
            odometry3dData_.motion.v.x = INSPVA_.data.north_vel;
            odometry3dData_.motion.v.y = INSPVA_.data.east_vel;
            // down = -up
            odometry3dData_.motion.v.z = -INSPVA_.data.up_vel;
            
            //attitude
            odometry3dData_.pose.o.r = INSPVA_.data.roll/180*M_PI;
            odometry3dData_.pose.o.p = INSPVA_.data.pitch/180*M_PI;
            // yaw is LH rule
            odometry3dData_.pose.o.y = INSPVA_.data.yaw/180*M_PI;
            
            //Set time
            // TODO: add this in if needed       
//             mkutctime(INSPVA_.data.week,
//                             INSPVA_.data.seconds,
//                             &pva_time);

            // set flag
            // newOdometry3dData_ = true;       
            
            odometry3dDataBuffer_.push( odometry3dData_ );
            
            return 0;       
            break;
        }
        case novatel::RAWIMUSB_LOG_TYPE:
        {
            // printf("got RAWIMUSB\n");
            memcpy(&RAWIMU_, &serial_data_.raw_message, sizeof(RAWIMU_) );

            imuData_.timeStamp = orcaice::toOrcaTime (timeOfRead_);
            
	    // Note scale factors and axis translation
            // TODO: are these gyro values correct?
	    double dt = 0.01;
	    // Divide by dt to get the correct units for the IMU data
            imuData_.gyro.x = -novatel::IMU_GYRO_CONST * (double)RAWIMU_.data.y_gyro/dt;
            imuData_.gyro.y = novatel::IMU_GYRO_CONST * (double)RAWIMU_.data.x_gyro/dt;
            imuData_.gyro.z = -novatel::IMU_GYRO_CONST * (double)RAWIMU_.data.z_gyro/dt;
            
            imuData_.accel.x = novatel::IMU_ACCEL_CONST * (double)RAWIMU_.data.y_accel/dt;
            imuData_.accel.y = -novatel::IMU_ACCEL_CONST * (double)RAWIMU_.data.x_accel/dt;
            imuData_.accel.z = novatel::IMU_ACCEL_CONST * (double)RAWIMU_.data.z_accel/dt;
            
            // TODO: add this if needed       
            //Set time
            // mkutctime(RAWIMU_.data.week,
            //          RAWIMU_.data.seconds,
            //          &raw_imu.time);

            //set flag
            // newImuData_ = true;   
            
            imuDataBuffer_.push( imuData_ );
                
            return 0;       
            break;
        }
        case novatel::INSCOVSB_LOG_TYPE:
        {
           // printf("got INSCOVSB\n");
           memcpy( &INSCOV_, &serial_data_.raw_message, sizeof(INSCOV_) );
// 
//             P_(0,0)=INSCOV_.data.pos_cov[0];
//             P_(2,0)=INSCOV_.data.pos_cov[1];
//             P_(4,0)=INSCOV_.data.pos_cov[2];
//             P_(0,2)=INSCOV_.data.pos_cov[3];
//             P_(2,2)=INSCOV_.data.pos_cov[4];
//             P_(4,2)=INSCOV_.data.pos_cov[5];
//             P_(0,4)=INSCOV_.data.pos_cov[6];
//             P_(2,4)=INSCOV_.data.pos_cov[7];
//             P_(4,4)=INSCOV_.data.pos_cov[8];
// 
//             P_(1,1)=INSCOV_.data.vel_cov[0];
//             P_(3,1)=INSCOV_.data.vel_cov[1];
//             P_(5,1)=INSCOV_.data.vel_cov[2];
//             P_(1,3)=INSCOV_.data.vel_cov[3];
//             P_(3,3)=INSCOV_.data.vel_cov[4];
//             P_(5,3)=INSCOV_.data.vel_cov[5];
//             P_(1,5)=INSCOV_.data.vel_cov[6];
//             P_(3,5)=INSCOV_.data.vel_cov[7];
//             P_(5,5)=INSCOV_.data.vel_cov[8];
// 
//             // attitude covariance is in degrees squared
//             const double deg2radcov = 1.0*M_PI*M_PI/(180*180);
//             P_(6,6)  = deg2radcov*INSCOV_.data.att_cov[0];
//             P_(8,6)  = deg2radcov*INSCOV_.data.att_cov[1];
//             P_(10,6) = deg2radcov*INSCOV_.data.att_cov[2];
//             P_(6,8)  = deg2radcov*INSCOV_.data.att_cov[3];
//             P_(8,8)  = deg2radcov*INSCOV_.data.att_cov[4];
//             P_(10,8) = deg2radcov*INSCOV_.data.att_cov[5];
//             P_(6,10) = deg2radcov*INSCOV_.data.att_cov[6];
//             P_(8,10) = deg2radcov*INSCOV_.data.att_cov[7];
//             P_(10,10)= deg2radcov*INSCOV_.data.att_cov[8];
// 
//             //Set Time
//             mkutctime(INSCOV_.data.week,
//                       INSCOV_.data.seconds,
//                       &cov_time);
//             // set flag
//             have_cov=true;
           return 0;
             break;
        }
        default:
        {
            printf("message %d not yet servicable\n",id);
            return -1;
            break;
        }
    }
    return -1;
}

int NovatelSpanInsGpsDriver::read_message( novatel_message* msg )
{
    // read the three sync bytes which are always at the start of the message header

    unsigned short id;
    unsigned long crc;
    unsigned long in_crc;
    msg->hdr.sop1 = 0;
    int skip = -1;
    int got;
    
    // read the first sync byte
    do{
        got = serial_->read_full( &msg->hdr.sop1, 1 );
        if ( got <= 0 )
        {
            // server has disconnected somehow
            return got;
        }
        if( got>0 )
        {
            skip++;
        }
    }while( msg->hdr.sop1 != 0xaa );

    if( skip>0 )
    {
        //printf("skipped %d bytes\n",skip);
    }
    
    // read the second sync byte
    do
    {
        got = serial_->read_full( &msg->hdr.sop2, 1 );
    }while( got!=1 );

    if( msg->hdr.sop2 != 0x44 )
    {
        return -1;
            //try again ??
    }

     // read the third sync byte
    do
    {
        got = serial_->read_full( &msg->hdr.sop3, 1 );
    }while( got != 1 );

    switch( msg->hdr.sop3 )
    {
        case 0x12: //long packet
            // how long is the header ?
            got = serial_->read_full( &msg->hdr.header_length, 1 );
            if( got<0 ) return -1;
            //printf("got %d bytes, asked for %d\n",got,1);
            
            // read all of the header...
            got = serial_->read_full( &msg->hdr.number, msg->hdr.header_length-4 );
            if( got<0 ) return -1;
            // printf("got %d bytes, asked for %d\n",got,msg->hdr.header_length-4);
            
            // read the  message data
            got = serial_->read_full( &msg->data, msg->hdr.length );
            if( got<0 ) return -1;
            //  printf("got %d bytes, asked for %d\n",got,msg->hdr.length);
            
            got = serial_->read_full( &in_crc, 4 );
            if( got<0 ) return -1;
            //printf("got %d bytes, asked for %d\n",got,1);
            
            id = msg->hdr.number;
            
            crc = novatel::CalculateCRC32( msg->raw_message,
                               msg->hdr.length+msg->hdr.header_length );
            break;

        case 0x13: //short packet
            // read rest of the header 12 bytes - 3 bytes already read
            got = serial_->read_full( &msg->short_hdr.length, 9 );
            if( got<0 ) return -1;
            // printf("got %d bytes, asked for %d\n",got,1);

            // read the rest of the message
            got = serial_->read_full( &msg->short_data, msg->short_hdr.length );
            if( got<0 ) return -1;
            // printf("got %d bytes, asked for %d\n",got,msg->short_hdr.length-4);

            got = serial_->read_full( &in_crc, 4 );
            if( got<0 ) return -1;
            //printf("got %d bytes, asked for %d\n",got,1);
            
            id = msg->short_hdr.number;

            crc = novatel::CalculateCRC32( msg->raw_message,msg->short_hdr.length + 12 );
            break;

        default: //bollocks
            return -1;
            break;
    }

    if(in_crc != crc)
    {
        fprintf( stderr,"CRC Error: 0x%lx, 0x%lx\n",in_crc,crc );
        return -1;
    }
    
    //printf("got message, id=%d\n",id);
    return  id;
}

int
NovatelSpanInsGpsDriver::mkutctime(int week, double seconds, struct timeval* tv)
{
//     if( !newGpsTime_ ){
//         return -1;
//     }

        // GPS Epoch is 0:00 hours 6th jan 1980
        // that is ten years 5 days plus two leap days
        // from the UNIX epoch.
        // currently minus 13 leap seconds

    int gpssecs=week*7*24*60*60 + (365*10+5+2)*24*60*60;

    double other_bits=pps_.offset+pps_.utc_offset+seconds;
    double other_bits_usecs=round(other_bits*1e6);

    tv->tv_sec=gpssecs+(int)floor(other_bits_usecs*1e-6);
    tv->tv_usec=(int)(other_bits_usecs-1e6*floor(other_bits_usecs*1e-6));

    fix_timeval(tv);

    return 0;
}

void
NovatelSpanInsGpsDriver::getImuAntennaOffsetProperties()
{
    Ice::PropertiesPtr prop = context_.properties();
    std::string prefix = context_.tag();
    prefix += ".Config.Novatel.";

    orcaice::setInit( imuAntennaOffset_ );
    orcaice::getPropertyAsCartesianPoint( prop, prefix+"ImuAntennaOffset", imuAntennaOffset_ );
    
    orcaice::setInit( imuAntennaOffsetUncertainty_ );
    orcaice::getPropertyAsCartesianPoint( prop, prefix+"ImuAntennaOffsetUncertainty", imuAntennaOffsetUncertainty_ );

    return;
}  

void
NovatelSpanInsGpsDriver::shutdown()
{
    // context_.tracer()->debug( "stopping driver", 5 );
    // orcaice::Thread::stopAndJoin( this );
    // context_.tracer()->debug( "stopped driver", 5 );
}               

} //namespace

