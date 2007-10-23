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
#include <orcagpsutil/latlon2mga.h>
#include "novatelspandriver.h"
#include "timeval.h"
#include "serialconnectivity.h"

// log types
#include <novatel/header.h>
#include <novatel/logtypes.h>
#include <novatel/crc/ncrc32.h>

#include <iostream>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

using namespace std;
using namespace orca;
using namespace hydroserial;
using namespace orcagpsutil;

namespace insgps
{

NovatelSpanInsGpsDriver::NovatelSpanInsGpsDriver( const char*             device,
                                                  int                     baud,
                                                  const Config&           cfg,
                                                  const orcaice::Context& context ) : 
    Driver(cfg, context),
    baud_(baud),
    serial_(0),
    enabled_( false ),
    // configure the buffers so they have depth 100 and are of type queue
    gpsDataBuffer_( 100 , hydroutil::BufferTypeQueue ),
    imuDataBuffer_( 100 , hydroutil::BufferTypeQueue ),
    odometry3dDataBuffer_( 100 , hydroutil::BufferTypeQueue ),
    localise3dDataBuffer_( 200 , hydroutil::BufferTypeQueue ),
    gpsCount_(0),
    imuCount_(0),
    localise3dCount_(0),
//     gpsData_(0),
//     imuData_(0),
//     localise3dData_(0),
    context_(context)
{
    const bool blockingMode = false;
    serial_ = new Serial( device, baud, blockingMode );
    //serial_->setDebugLevel(1);

    // set to n second timeout
    serial_->setTimeout( 2, 0 );
    // hasFix_       = false;
    // havePps_      = false;
    // havePosition_ = false;
    // havePva_      = false;
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

    //context_.tracer()->debug( "!_DOES__NOTHING_!_ NovatelSpanInsGps: resetting Novatel Span InsGps driver", 2 );

    // baudrates we test for; this is
    // _not_ all the baudrates the receiver
    // can possible be set to
    int baudrates[]={
        9600,
        19200,
        38400,
        57600,
        115200,
        230400
    };
    int currentBaudrate = 0;
    bool correctBaudrate = false;

    std::cout << "Trying to hook up to receiver at different Baudrates\n";
    int maxTry = 4;
    int successThresh = 4;
    std::string challenge("unlogall\r\n");
    std::string ack("<OK");
    size_t i=0;
    while(false == correctBaudrate && (i<sizeof baudrates)){
        currentBaudrate = baudrates[i];
        correctBaudrate = testConnectivity( challenge, ack, serial_, 100, maxTry, successThresh, currentBaudrate);
        i++;
    }
    if(false == correctBaudrate){
        return -1;
    }
    char str[256];
    sprintf( str,"com com1 %d n 8 1 n off on\r\n", baud_ );
    serial_->writeString( str );
    std::cout << "*******************************\n"
        << "** Current Speed " << currentBaudrate << "\n"
        << "** Resetting to " << baud_ << "\n"
        << "*******************************\n";
    std::cout << "** Testing new setting\n** ";
    if(true == testConnectivity( challenge, ack, serial_, 100, maxTry, successThresh, baud_)){
        std::cout << "*******************************\n";
        return 0;
    }else{
        std::cout << "*******************************\n";
        return -1;
    }
}

void
NovatelSpanInsGpsDriver::enable()
{
    // start the orcaice::thread for this driver 
    start();
    
    if ( init() < 0 )
    {
        context_.tracer()->error("Couldn't initialise Novatel serial driver");
        enabled_ = false;
    }               
    else
    {      
        context_.tracer()->info("Driver initialised");
        enabled_ = true;
    }      
}

int
NovatelSpanInsGpsDriver::init()
{
    if ( enabled_ )
        return 0;

    context_.tracer()->info("NovatelSpanInsGps: Initialising Novatel Span InsGps driver");

#ifdef __QNX__
    // get the priority of this thread
    threadPriorityLow_ = getprio( 0 );
    // use this in getprio() for dynamically increasing the priority of
    // this thread by 4
    threadPriorityHigh_ =  threadPriorityLow_ + 18;
#endif

    //
    // send initialisation commands to the Novatel   
    //

    int put;
    
    // just in case something is running... stops the novatel logging any messages
    // serial_->flush();
    put = serial_->writeString( "unlogall\r\n" );
    //printf("put %d bytes\n",put);
    serial_->drain();

    //
    // IMU Specific settings
    //
    // tell the novatel what serial port the imu is attached to (com3 = aux)
    put = serial_->writeString( "interfacemode com3 imu imu on\r\n" );
    // the type of imu being used
    put = serial_->writeString( "setimutype imu_hg1700_ag62\r\n" );
    //force the IMU to re-align at every startup
    put = serial_->writeString( "inscommand reset\r\n" );
    context_.tracer()->info("Reset IMU; Waiting 5 seconds before continuing!");
    sleep(5);

    // read and set the imu params (orientation of imu and offset to gps antenna)
    setNovatelSpecificParams();

    // for dgps
    // put = serial_->writeString( "com com2 57600 n 8 1 n off on\r\n" ); // My Addition
    // put = serial_->writeString( "interfacemode com2 rtca none off\r\n" ); // My Addition

    // turn off posave as this command implements position averaging for base stations.
    put = serial_->writeString( "posave off\r\n" );

    // make sure that fixposition has not been set
    put = serial_->writeString( "fix none\r\n" );

    // select the geodetic datum for operation of the receiver (wgs84 = default)
    put = serial_->writeString( "datum wgs84\r\n" );

    /////////////////////////////////////////////////
    // tell the receiver what kind of info it should spit out
    /////////////////////////////////////////////////

    ////////////////////////////////////////
    // read log params from the config file
    Ice::PropertiesPtr logProp = context_.properties();
    std::string prefix = context_.tag();
    prefix += ".Config.Novatel.LogEnable.";
    int enableMsg;

    // receiver status
    enableMsg = orcaice::getPropertyAsIntWithDefault( logProp, prefix+"rxstatusb", 0 );
    if(enableMsg){
        context_.tracer()->info("turning on receiver status!");
        put = serial_->writeString( "log rxstatusb ontime 1.0\r\n" );
    }

    // PPS pulse will be triggered before this arrives
    // time used to sync with the pps signal
    enableMsg = orcaice::getPropertyAsIntWithDefault( logProp, prefix+"timesyncb", 0 );
    if(enableMsg){
        context_.tracer()->info("Turning on PPS!");
        put = serial_->writeString( "log timeb ontime 1.0\r\n" );
        put = serial_->writeString( "log timesyncb ontime 1.0\r\n");
    }

    // gps position without ins
    enableMsg = orcaice::getPropertyAsIntWithDefault( logProp, prefix+"bestgpsposb", 0 );
    if(enableMsg){
        context_.tracer()->info("Turning on GPS position at 5Hz!");
        put = serial_->writeString( "log bestgpsposb ontime 0.2\r\n" );
    }

    //////////////
    // IMU message
    ///////////////

    // short IMU messages
    // pva data in wgs84 coordinates
    enableMsg = orcaice::getPropertyAsIntWithDefault( logProp, prefix+"inspvasb", 1 );
    if(enableMsg){
        context_.tracer()->info("Turning on INS position/velocity/oriantation at 100Hz!");
        put = serial_->writeString( "log inspvasb ontime 0.01\r\n" );
    }

    // raw accelerometer and gyro data
    enableMsg = orcaice::getPropertyAsIntWithDefault( logProp, prefix+"rawimusb", 0 );
    if(enableMsg){
        // pva data in wgs84 coordinates
        sleep(1);
        enableMsg = orcaice::getPropertyAsIntWithDefault( logProp, prefix+"inspvasb", 1 );
        if(enableMsg){
            context_.tracer()->info("!!We want raw IMU as well!!\n;Turning on INS position/velocity/oriantation at 50Hz only!");
            put = serial_->writeString( "log inspvasb ontime 0.02\r\n" );
        }
        context_.tracer()->info("Turning on raw imu data!");
        put = serial_->writeString( "log rawimusb onnew\r\n" );
    }

    // pva covariances
    enableMsg = orcaice::getPropertyAsIntWithDefault( logProp, prefix+"inscovsb", 0 );
    if(enableMsg){
        context_.tracer()->info("Turning on INS covariances!");
        put = serial_->writeString( "log inscovsb onchanged\r\n" );
    }

    /////////////////////////////////////////////////
    // turn special receiver modes on/off
    /////////////////////////////////////////////////

    ////////////////////////////////////////
    // read mode params from the config file
    Ice::PropertiesPtr modeProp = context_.properties();
    std::string modePrefix = context_.tag();
    modePrefix += ".Config.Novatel.Mode.";
    int enableMode;

    //CDGPS
    enableMode = orcaice::getPropertyAsIntWithDefault( modeProp, modePrefix+"CDGPS", 0 );
    if(enableMode){
        context_.tracer()->info("Turning on CDGPS!");
        put = serial_->writeString( "ASSIGNLBAND CDGPS 1547547 4800\r\n" );
    }

    //!!!! NOT active at the moment; might invalidate our OmniSTAR setup !!!!!
    //OmniSTAR
    //enableMode = orcaice::getPropertyAsIntWithDefault( modeProp, modePrefix+"OmniSTAR", 0 );
    //if(enableMode){
        //cout << "turning on OmniSTAR!" << endl;
        //put = serial_->writeString( "ASSIGNLBAND OMNISTAR 1536782 1200\r\n" );
    //}
    //!!!! NOT active at the moment; might invalidate our OmniSTAR setup !!!!!

    //rtk
    enableMode = orcaice::getPropertyAsIntWithDefault( modeProp, modePrefix+"rtk", 0 );
    if(enableMode){
        context_.tracer()->info("Turning on RTK!");
        put = serial_->writeString( "com com2,9600,n,8,1,n,off,on\r\n" );
        put = serial_->writeString( "interfacemode com2 rtca none\r\n" );
    }

    //roll pitch correction
    enableMode = orcaice::getPropertyAsIntWithDefault( modeProp, modePrefix+"swappedRollPitch", 0 );
    if(enableMode){
        context_.tracer()->info("Turning ON roll/pitch swap!");
        context_.tracer()->warning("!!! Swapping roll and pitch in odometry3d, localise3d and respective rates in imu !!!");
        swappedRollPitch_=true;
    }else{
        context_.tracer()->info("Turning OFF roll/pitch swap!");
        context_.tracer()->warning("!!! NOT swapping roll/pitch !!!");
        swappedRollPitch_=false;
    }

    // turn SBAS on/off (essentially global DGPS)
    enableMode = orcaice::getPropertyAsIntWithDefault( modeProp, modePrefix+"SBAS", 0 );
    if(enableMode){
        context_.tracer()->info("Turning on SBAS!");
        put = serial_->writeString( "SBASCONTROL ENABLE Auto 0 ZEROTOTWO\r\n");
        //we try to use WAAS satellites even below the horizon
        put = serial_->writeString( "WAASECUTOFF -5.0\r\n");
    }
    else{
        context_.tracer()->info("Turning off SBAS!");
        put = serial_->writeString( "SBASCONTROL DISABLE Auto 0 NONE\r\n");
    }

    //Let the receiver figure out which corrections are best
    //put = serial_->writeString( "RTKSOURCE AUTO\r\n" );

    //We only use our own rtk corrections; _not_ OmniSTAR HP
    put = serial_->writeString( "RTKSOURCE RTCA ANY\r\n" );
    put = serial_->writeString( "PSRDIFFSOURCE AUTO\r\n" );

    start();
    enabled_ = true;

    return 0;
}

int
NovatelSpanInsGpsDriver::disable()
{
//    cout << "enabled_: " << enabled_ << endl;

    if ( !enabled_ ) return 0;

    context_.tracer()->info("Disabling Novatel Span InsGps driver");
 
    int put;

    // just in case something is running... stops the novatel logging any messages
    serial_->flush();
    put = serial_->writeString( "unlogall\r\n" );
    //printf("put %d bytes\n",put);
    serial_->drain();

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
        context_.tracer()->debug( "novatelspandriver::readGps(): Timeout while waiting for GPS packet", 6 );
    }
    else
    {
        context_.tracer()->debug( "novatelspandriver::readGps(): got gps data", 6 );

        // cout << "gpsCount_: " << gpsCount_ << endl;
        if (gpsCount_ > 2000 )
        {
            std::string str = "Gps Data Buffer is " + gpsDataBuffer_.size()/100;
            str += "% full";
            context_.tracer()->info( str, 6 );
            gpsCount_ = 0;
        }
        gpsCount_++;
    }

    return;
}

void
NovatelSpanInsGpsDriver::readGpsTime( orca::GpsTimeData& data, int timeoutMs )
{
    context_.tracer()->info( "novatelspandriver::readGpsTime(): GpsTime is not provided in this driver", 6 );
    return;
}

void
NovatelSpanInsGpsDriver::readImu( orca::ImuData& data, int timeoutMs )
{
    // blocking read with timeout. Also deletes the front element from the buffer
    int ret = imuDataBuffer_.getAndPopNext( data, timeoutMs );
    //printf("%s gyro.x %f\n", __PRETTY_FUNCTION__, data.gyro.x);
    if ( ret != 0 ) {
        // throw NovatelSpanException( "Timeout while waiting for IMU packet" );
        context_.tracer()->debug( "novatelspandriver::readImu(): Timeout while waiting for IMU packet", 6 );
    }
    else
    {
        context_.tracer()->debug( "novatelspandriver::readImu(): got imu data", 6 );

        if (imuCount_ > 2000 )
        {
            std::string str = "Imu Data Buffer is " + imuDataBuffer_.size()/100;
            str += "% full";
            context_.tracer()->info( str, 6 );
            imuCount_ = 0;
        }
        imuCount_++;

    }

    return;
}

void
NovatelSpanInsGpsDriver::readOdometry3d( orca::Odometry3dData& data, int timeoutMs )
{
    // Note that this driver only provides localise3d and not odometry3d.
    // This is here to keep the main driver happy as it is pure virtual

    // blocking read with timeout. Also deletes the front element from the buffer
    int ret = odometry3dDataBuffer_.getAndPopNext( data, timeoutMs );
    if ( ret != 0 ) {
        // throw NovatelSpanException( "Timeout while waiting for Odometry3d packet" );
        context_.tracer()->info( "novatelspandriver::readOdometry3d()): odometry3d is not provided by this driver", 6 );
    }
    else
    {
        context_.tracer()->debug( "novatelspandriver::readOdometry3d()): got odometry3d data", 6 );
    }

    return;
}

void
NovatelSpanInsGpsDriver::readLocalise3d( orca::Localise3dData& data, int timeoutMs )
{
    // blocking read with timeout. Also deletes the front element from the buffer
    int ret = localise3dDataBuffer_.getAndPopNext( data, timeoutMs );
    if ( ret != 0 ) {
        // throw NovatelSpanException( "Timeout while waiting for Localise3d packet" );
        context_.tracer()->debug( "novatelspandriver::readLocalise3d()): Timeout while waiting for Localise3d packet", 6 );
    }
    else
    {
        context_.tracer()->debug( "novatelspandriver::readLocalise3d()): got localise3d data", 6 );

        if (localise3dCount_ > 2000 )
        {
            std::string str = "Localise3d Data Buffer is " + localise3dDataBuffer_.size()/100;
            str += "% full";
            context_.tracer()->info( str, 6 );
            localise3dCount_ = 0;
        }
        localise3dCount_++;

    }

    return;
}

void
NovatelSpanInsGpsDriver::run()
{
    try
    {

    int retMsgs; 
        // We can't block in this loop -- have to keep it rolling so
        // that !isStopping() is always checked.
        while ( !isStopping() )
        {
            try
            {
                #ifdef __QNX__
                    setprio( 0, threadPriorityHigh_ );
                #endif

                // Guaranteed not to block for long.
                int ret = 0;
                try{
                    ret = serial_->bytesAvailableWait();
                }
                catch ( const std::string &e )
                {
                    std::stringstream ss;
                    ss << "ERROR("<<__FILE__<<" _wait_ ): Caught unexpected string: " << e;
                    context_.tracer()->error( ss.str() );
                    //context_.status()->fault( SUBSYSTEM, ss.str() );            
                }
                // timeOfRead_ = IceUtil::Time::now();
                if ( ret < 0 )
                {
                    throw( std::string("Error waiting for data: ")+strerror(errno) );
                }
                else if ( ret > 0 )
                {
                    // timeOfRead_ = IceUtil::Time::now();
                    retMsgs = readMsgsFromHardware();
                    // cout << "retMsgs: " << retMsgs << endl; 
                }
                else
                {
                     throw NovatelSpanException("Timed out while waiting for data: ");
                }

                #ifdef __QNX__
                    setprio( 0, threadPriorityLow_ );
                #endif
            }
            catch ( NovatelSpanException &e )
            {
                std::stringstream ss;
                ss<<"Caught NovatelSpanException: " << e.what();
                context_.tracer()->warning( ss.str() );
            }
            catch ( SerialException &e )
            {
                std::stringstream ss;
                ss<<"Caught SerialException: " << e.what();
                context_.tracer()->warning( ss.str() );
            }
            catch ( hydroutil::Exception & e )
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
            catch ( std::exception & e )
            {
                std::stringstream ss;
                ss << "novatelspandriver::run(): Caught std::exception: " << e.what();
                context_.tracer()->warning( ss.str() );
            }
            catch ( const std::string &e )
            {
                std::stringstream ss;
                ss << "ERROR("<<__FILE__<<"): Caught unexpected string: " << e;
                context_.tracer()->error( ss.str() );
                //context_.status()->fault( SUBSYSTEM, ss.str() );            
            }
            catch ( const char *e )
            {
                std::stringstream ss;
                ss << "ERROR("<<__FILE__<<"): Caught unexpected char *: " << e;
                context_.tracer()->error( ss.str() );
                //context_.status()->fault( SUBSYSTEM, ss.str() );
            }
            catch ( ... )
            {
                context_.tracer()->warning( "Caught some other exception..." );
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
        context_.tracer()->info( "NovatelSpanInsGps: ERROR: Can't read: not enabled . Sleeping for 1 sec...", 6 );
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(1));
        return -1;
    }

    int ret;
    int msgs = 0;

    // read novatel binary messages
    ret = read_message( &serial_data_ );
    //cout << "readMsgsFromHardware(): ret: " << ret << endl;
    // ret = serial_->read_line(serial_data_,1024,'\n');

    if( ret>0 )
    {

    // cout << "id: " << ret << endl; 
    if( populateData( ret ) == 0 )
        {
            msgs++;
        }
    }

    if ( ret <= 0 )
    {
        if( errno != EAGAIN )
        {
            std::string errString = "Error reading from InsGps: ";
            errString += strerror(errno);
            errString += "--shutting down.";
            throw hydroutil::Exception( ERROR_INFO, errString );
            // cout << "NovatelSpanInsGps: ERROR: Error reading from InsGps:" << strerror(errno) << " -- shutting down." << endl;
            enabled_ = false;
            return -1;
        }
    }

    //return how many messages read
    return msgs;
}

orca::GpsPositionType
convertPosType( unsigned int novatelPosType )
{
    switch(novatelPosType){
        case 0: cout <<"GPS pos type: NONE" <<endl;
                return orca::NovatelNone;
                break;
        case 1: cout <<"GPS pos type: FIXEDPOS" <<endl;
                return orca::NovatelFixedPos;
                break;
        case 2: cout <<"GPS pos type: FIXEDHEIGHT" <<endl;
                return orca::NovatelFixedHeigth;
                break;
        case 4: cout <<"GPS pos type: FLOATCONV" <<endl;
                return orca::NovatelFloatConv;
                break;
        case 5: cout <<"GPS pos type: WIDELANE" <<endl;
                return orca::NovatelWideLane;
                break;
        case 6: cout <<"GPS pos type: NARROWLANE" <<endl;
                return orca::NovatelNarrowLane;
                break;
        case 8: cout <<"GPS pos type: DOPPLER_VELOCITY" <<endl;
                return orca::NovatelDopplerVelocity;
                break;
        case 16: cout <<"GPS pos type: SINGLE" <<endl;
                return orca::NovatelSingle;
                 break;
        case 17: cout <<"GPS pos type: PSRDIFF" <<endl;
                return orca::NovatelPsrDiff;
                 break;
        case 18: cout <<"GPS pos type: WAAS" <<endl;
                return orca::NovatelWAAS;
                 break;
        case 19: cout <<"GPS pos type: PROPOGATED" <<endl;
                return orca::NovatelPropagated;
                 break;
        case 20: cout <<"GPS pos type: OMNISTAR" <<endl;
                return orca::NovatelOmnistar;
                 break;
        case 32: cout <<"GPS pos type: L1_FLOAT" <<endl;
                return orca::NovatelL1Float;
                 break;
        case 33: cout <<"GPS pos type: IONFREE_FLOAT" <<endl;
                return orca::NovatelIonFreeFloat;
                 break;
        case 34: cout <<"GPS pos type: NARROW_FLOAT" <<endl;
                return orca::NovatelNarrowFloat;
                 break;
        case 48: cout <<"GPS pos type: L1_INT" <<endl;
                return orca::NovatelL1Int;
                 break;
        case 49: cout <<"GPS pos type: WIDE_INT" <<endl;
                return orca::NovatelWideInt;
                 break;
        case 50: cout <<"GPS pos type: NARROW_INT" <<endl;
                return orca::NovatelNarrowInt;
                 break;
        case 51: cout <<"GPS pos type: RTK_DIRECT_INS" <<endl;
                return orca::NovatelRTKDirectINS;
                 break;
        case 52: cout <<"GPS pos type: INS" <<endl;
                return orca::NovatelINS;
                 break;
        case 53: cout <<"GPS pos type: INS_PSRSP" <<endl;
                return orca::NovatelINSPSRSP;
                 break;
        case 54: cout <<"GPS pos type: INS_PSRFLOAT" <<endl;
                return orca::NovatelINSPSRFLOAT;
                 break;
        case 55: cout <<"GPS pos type: INS_RTKFLOAT" <<endl;
                return orca::NovatelINSRTKFLOAT;
                 break;
        case 56: cout <<"GPS pos type: INS_RTKFIXED" <<endl;
                return orca::NovatelINSRTKFIXED;
                 break;
        case 64: cout <<"GPS pos type: OMNISTAR_HP" <<endl;
                return orca::NovatelOmnistarHP;
                 break;
        default: cout <<"GPS pos type: Unknown: "<<novatelPosType <<endl;
                return orca::NovatelUnknown;
                 break;
    };
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
                context_.tracer()->info( "UTC time not available", 8 );
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

            gpsData_.positionType = convertPosType(BESTGPSPOS_.data.pos_type);
            if( gpsData_.positionType == NovatelNone)
            {
                // newGpsData_ = false;
                context_.tracer()->debug( "Position not yet available", 8 );
                return -1;
            }

            //Set time
            // gettimeofday(&position_.time,NULL);
            gpsData_.timeStamp = orcaice::toOrcaTime(timeOfRead_);

            // TODO: made up the zone here... need to read the real zone         
             int zone = 0;
            position_.zone = zone;
            // double lat = BESTGPSPOS_.data.latitude;
            gpsData_.latitude = BESTGPSPOS_.data.latitude;
            // double lng = BESTGPSPOS_.data.longitude;
            gpsData_.longitude = BESTGPSPOS_.data.longitude;
            // position_.down = -BESTGPSPOS_.data.height;
            gpsData_.altitude = BESTGPSPOS_.data.height;

            // depends on the number of satellites... not sure how to include this in
            // the gps interface       
            sol_status_ = BESTGPSPOS_.data.sol_status;

            // TODO: are the no. of obs the same as satellites?
            gpsData_.satellites = BESTGPSPOS_.data.num_GPSL1;
            gpsData_.observationCountOnL1 = BESTGPSPOS_.data.num_L1;
            gpsData_.observationCountOnL2 = BESTGPSPOS_.data.num_L2;

            static int gpsStatusCnt;
            if(gpsStatusCnt++>=5){
                gpsStatusCnt = 0;
                cout << "   Number of obs: " << (int)(BESTGPSPOS_.data.num_obs)
                    <<" GPSL1: "
                    <<(int)(BESTGPSPOS_.data.num_GPSL1)
                    <<" L1: "
                    <<(int)(BESTGPSPOS_.data.num_L1)
                    <<" L2: "
                    << (int)(BESTGPSPOS_.data.num_L2)
                    << endl;
                cout << "   Solution age: " << BESTGPSPOS_.data.sol_age <<  endl;
                cout << "   Diff age: " << BESTGPSPOS_.data.diff_age <<  endl;
            }


            //LatLon2MGA(lat,lng,
            //           position_.northing, position_.easting, zone);


            // set flag
            // newGpsData_ = true;

            // context_.tracer()->debug( "TRACE(novatelspandriver::populateData()): Pushing gps data to buffer", 5);
            gpsDataBuffer_.push( gpsData_ );
            // context_.tracer()->debug( "TRACE(novatelspandriver::populateData()): Pushed gps data to buffer", 5);

            return 0;       
            break;
        }
        case novatel::INSPVASB_LOG_TYPE:
        //this gets stuffed into localise3d _and_ odometry3d; probably not the best way to go
        //angular motion in odometry3d is _not_ populated
        {
            // printf("got INSPVASB\n");
            memcpy( &INSPVA_, &serial_data_.raw_message, sizeof(INSPVA_) );

            // cout << "GPS time: " << INSPVA_.data.seconds << endl;
            //    printf("%10.10f\n",INSPVA_.data.seconds); 
            localise3dData_.timeStamp = orcaice::toOrcaTime(timeOfRead_);
            odometry3dData_.timeStamp = orcaice::toOrcaTime(timeOfRead_);
                //cout << "timeOfRead_: " << orcaice::toString(localise3dData_.timeStamp) << endl; 

            // load the pva data into the localise3d object       
            if ( localise3dData_.hypotheses.size() == 0 )
            {
                localise3dData_.hypotheses.resize(1);
                localise3dData_.hypotheses[0].weight = 1;
            }
            // cout << "lattitude and longitude: " << INSPVA_.data.latitude << " " << INSPVA_.data.longitude << endl;

            localise3dData_.hypotheses[0].mean.p.x = INSPVA_.data.longitude;
            localise3dData_.hypotheses[0].mean.p.y = INSPVA_.data.latitude;
            localise3dData_.hypotheses[0].mean.p.z = INSPVA_.data.height;

            odometry3dData_.pose.p.x = INSPVA_.data.longitude;
            odometry3dData_.pose.p.y = INSPVA_.data.latitude;
            odometry3dData_.pose.p.z = INSPVA_.data.height;

            // velocities
            odometry3dData_.motion.v.x = INSPVA_.data.east_vel;
            odometry3dData_.motion.v.y = INSPVA_.data.north_vel;
            // v.z used to be flipped; didn't match when integrated
            odometry3dData_.motion.v.z = INSPVA_.data.up_vel;

            if(true == swappedRollPitch_){
                //attitude
                localise3dData_.hypotheses[0].mean.o.r = INSPVA_.data.pitch/180*M_PI;
                localise3dData_.hypotheses[0].mean.o.p = INSPVA_.data.roll/180*M_PI;
                // yaw is LH rule
                localise3dData_.hypotheses[0].mean.o.y = 2*M_PI - INSPVA_.data.yaw/180*M_PI;

                odometry3dData_.pose.o.r = INSPVA_.data.pitch/180*M_PI;
                odometry3dData_.pose.o.p = INSPVA_.data.roll/180*M_PI;
                // yaw is LH rule
                odometry3dData_.pose.o.y = 2*M_PI - INSPVA_.data.yaw/180*M_PI;
            }else{
                //attitude
                localise3dData_.hypotheses[0].mean.o.r = INSPVA_.data.roll/180*M_PI;
                localise3dData_.hypotheses[0].mean.o.p = INSPVA_.data.pitch/180*M_PI;
                // yaw is LH rule
                localise3dData_.hypotheses[0].mean.o.y = 2*M_PI - INSPVA_.data.yaw/180*M_PI;

                odometry3dData_.pose.o.r = INSPVA_.data.roll/180*M_PI;
                odometry3dData_.pose.o.p = INSPVA_.data.pitch/180*M_PI;
                // yaw is LH rule
            }


            //Set time
            // TODO: add this in if needed       
//             mkutctime(INSPVA_.data.week,
//                             INSPVA_.data.seconds,
//                             &pva_time);

            localise3dDataBuffer_.push( localise3dData_ );
            odometry3dDataBuffer_.push( odometry3dData_ );

            static int insStatusCnt;
            if(insStatusCnt++ >=100){
                insStatusCnt = 0;
                context_.tracer()->info( insStatusToString( INSPVA_.data.status ), 1 );
            }

            return 0;       
            break;
        }
        case novatel::INSCOVSB_LOG_TYPE:
        {
           // printf("got INSCOVSB\n");
           memcpy( &INSCOV_, &serial_data_.raw_message, sizeof(INSCOV_) );

//             P_(1,1)=INSCOV_.data.vel_cov[0];
//             P_(3,1)=INSCOV_.data.vel_cov[1];
//             P_(5,1)=INSCOV_.data.vel_cov[2];
//             P_(1,3)=INSCOV_.data.vel_cov[3];
//             P_(3,3)=INSCOV_.data.vel_cov[4];
//             P_(5,3)=INSCOV_.data.vel_cov[5];
//             P_(1,5)=INSCOV_.data.vel_cov[6];
//             P_(3,5)=INSCOV_.data.vel_cov[7];
//             P_(5,5)=INSCOV_.data.vel_cov[8];

	    if ( localise3dData_.hypotheses.size() == 0 )
	    {
		localise3dData_.hypotheses.resize(1);
                localise3dData_.hypotheses[0].weight = 1;
	    }

	    // don't need to store the whole matrix as it is symmetric
	    localise3dData_.hypotheses[0].cov.xx = INSCOV_.data.pos_cov[0];
	    localise3dData_.hypotheses[0].cov.xy = INSCOV_.data.pos_cov[1];
	    localise3dData_.hypotheses[0].cov.xz = INSCOV_.data.pos_cov[2];
	    // localise3dData_.hypotheses[0].cov.yx = INSCOV_.data.pos_cov[3];
	    localise3dData_.hypotheses[0].cov.yy = INSCOV_.data.pos_cov[4];
	    localise3dData_.hypotheses[0].cov.yz = INSCOV_.data.pos_cov[5];
	    // localise3dData_.hypotheses[0].cov.zx = INSCOV_.data.pos_cov[6];
	    // localise3dData_.hypotheses[0].cov.zy = INSCOV_.data.pos_cov[7];
	    localise3dData_.hypotheses[0].cov.zz = INSCOV_.data.pos_cov[8];
	
	    // the novatel messages do not consider correlations between angular and linear motion
	    localise3dData_.hypotheses[0].cov.xr = 0;
	    localise3dData_.hypotheses[0].cov.xp = 0;
	    localise3dData_.hypotheses[0].cov.xa = 0;
	    localise3dData_.hypotheses[0].cov.yr = 0;
	    localise3dData_.hypotheses[0].cov.yp = 0;
	    localise3dData_.hypotheses[0].cov.ya = 0;
	    localise3dData_.hypotheses[0].cov.zr = 0;
	    localise3dData_.hypotheses[0].cov.zp = 0;
	    localise3dData_.hypotheses[0].cov.za = 0;
	
	    // attitude covariance is in degrees squared
	    const double deg2radcov = 1.0*M_PI*M_PI/(180*180);
	    localise3dData_.hypotheses[0].cov.rr  = deg2radcov*INSCOV_.data.att_cov[0];
	    localise3dData_.hypotheses[0].cov.rp  = deg2radcov*INSCOV_.data.att_cov[1];
	    localise3dData_.hypotheses[0].cov.ra  = deg2radcov*INSCOV_.data.att_cov[2];
	    // localise3dData_.hypotheses[0].cov.pr  = deg2radcov*INSCOV_.data.att_cov[3];
	    localise3dData_.hypotheses[0].cov.pp  = deg2radcov*INSCOV_.data.att_cov[4];
	    localise3dData_.hypotheses[0].cov.pa  = deg2radcov*INSCOV_.data.att_cov[5];
	    // localise3dData_.hypotheses[0].cov.ar  = deg2radcov*INSCOV_.data.att_cov[6];
	    // localise3dData_.hypotheses[0].cov.ap  = deg2radcov*INSCOV_.data.att_cov[7];
	    localise3dData_.hypotheses[0].cov.aa  = deg2radcov*INSCOV_.data.att_cov[8];

//             //Set Time
//             mkutctime(INSCOV_.data.week,
//                       INSCOV_.data.seconds,
//                       &cov_time);
//             // set flag
//             have_cov=true;
           return 0;
             break;
        }
        case novatel::RAWIMUSB_LOG_TYPE:
        {
            //printf("got RAWIMUSB\n");
            memcpy(&RAWIMU_, &serial_data_.raw_message, sizeof(RAWIMU_) );

            imuData_.timeStamp = orcaice::toOrcaTime(timeOfRead_);

	    // Note scale factors and axis translation
            // TODO: are these gyro values correct?
	    double dt = 0.01;
	    // Divide by dt to get the correct units for the IMU data
            if(true == swappedRollPitch_){
                imuData_.gyro.y = -novatel::IMU_GYRO_CONST * (double)RAWIMU_.data.y_gyro/dt;
                imuData_.gyro.x = novatel::IMU_GYRO_CONST * (double)RAWIMU_.data.x_gyro/dt;
            }else{
                imuData_.gyro.x = -novatel::IMU_GYRO_CONST * (double)RAWIMU_.data.y_gyro/dt;
                imuData_.gyro.y = novatel::IMU_GYRO_CONST * (double)RAWIMU_.data.x_gyro/dt;
            }
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

            //printf("%s gyro.x %f\n", __PRETTY_FUNCTION__, imuData_.gyro.x);
            imuDataBuffer_.push( imuData_ );

            return 0;
            break;
        }
        default:
        {
            std::string str = "message " + id;
            str += "not yet servicable";
            context_.tracer()->info( str, 8 );
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
        got = serial_->read( &msg->hdr.sop1, 1 );
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

    // get timestamp after the first byte for accuracy
    timeOfRead_ = IceUtil::Time::now();

   if( skip>0 )
    {
        //printf("skipped %d bytes\n",skip);
    }

    // read the second sync byte
    do
    {
        got = serial_->readFull( &msg->hdr.sop2, 1 );
    }while( got!=1 );

    if( msg->hdr.sop2 != 0x44 )
    {
        return -1;
            //try again ??
    }

     // read the third sync byte
    do
    {
        got = serial_->readFull( &msg->hdr.sop3, 1 );
    }while( got != 1 );

    switch( msg->hdr.sop3 )
    {
        case 0x12: //long packet
            // how long is the header ?
            got = serial_->readFull( &msg->hdr.header_length, 1 );
            //printf("got %d bytes, asked for %d\n",got,1);

            // read all of the header...
            got = serial_->readFull( &msg->hdr.number, msg->hdr.header_length-4 );
            // printf("got %d bytes, asked for %d\n",got,msg->hdr.header_length-4);

            // read the  message data
            got = serial_->readFull( &msg->data, msg->hdr.length );
            //  printf("got %d bytes, asked for %d\n",got,msg->hdr.length);

            got = serial_->readFull( &in_crc, 4 );
            //printf("got %d bytes, asked for %d\n",got,1);

            id = msg->hdr.number;

            crc = novatel::CalculateCRC32( msg->raw_message,
                               msg->hdr.length+msg->hdr.header_length );
            break;

        case 0x13: //short packet
            // read rest of the header 12 bytes - 3 bytes already read
            got = serial_->readFull( &msg->short_hdr.length, 9 );
            // printf("got %d bytes, asked for %d\n",got,1);

            // read the rest of the message
            got = serial_->readFull( &msg->short_data, msg->short_hdr.length );
            // printf("got %d bytes, asked for %d\n",got,msg->short_hdr.length-4);

            got = serial_->readFull( &in_crc, 4 );
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
        throw NovatelSpanException( "CRC Error" );
        return -1;
    }

    //printf("got message, id=%d\n",id);
    return  id;
}

std::string
NovatelSpanInsGpsDriver::insStatusToString( const int& status )
{
    std::string str;

    switch( status )
    {
        case 0:
            str = "Ins Status: Ins is inactive";
            break;
        case 1:
            str = "Ins Status: Ins is aligning";
            break;
        case 2:
            str = "Ins Status: Ins solution is bad";
            break;
        case 3:
            str = "Ins Status: Ins solution is good";
            break;
        // 4 and 5 are reserved
        case 6:
            str = "Ins Status: Bad Ins Gps agreement";
            break;
        case 7:
            str = "Ins Status: Ins alignment is complete but vehicle must perform maneuvers so that the attitude can converge";
            break;
        default:
            str = "Ins Status: Unknown Ins Status";
            break;
     }

    return str;

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
NovatelSpanInsGpsDriver::setNovatelSpecificParams()
{
    ////////////////////////////////////////
    // read our params from the config file
    Ice::PropertiesPtr prop = context_.properties();
    std::string prefix = context_.tag();
    prefix += ".Config.Novatel.";

    // imu orientation constant
    int imuOrientation = orcaice::getPropertyAsIntWithDefault( prop, prefix+"ImuOrientation", 0 );

    // vehicle to imu body rotation
    orca::CartesianPoint imuVehicleBodyRotation;
    orcaice::setInit( imuVehicleBodyRotation );
    orcaice::getPropertyAsCartesianPoint( prop, prefix+"VehicleBodyRotation", imuVehicleBodyRotation);

    // vehicle to imu body rotation uncertainty
    orca::CartesianPoint imuVehicleBodyRotationUncertainty;
    orcaice::setInit( imuVehicleBodyRotationUncertainty );
    orcaice::getPropertyAsCartesianPoint( prop, prefix+"VehicleBodyRotationUncertainty",
                                          imuVehicleBodyRotationUncertainty);

    // imuToGpsAntennaOffset
    orca::CartesianPoint imuToGpsAntennaOffset;
    orcaice::setInit( imuToGpsAntennaOffset );
    orcaice::getPropertyAsCartesianPoint( prop, prefix+"ImuToGpsAntennaOffset", imuToGpsAntennaOffset );

    // imuToGpsAntennaOffsetUncertainty
    orca::CartesianPoint imuToGpsAntennaOffsetUncertainty;
    orcaice::setInit( imuToGpsAntennaOffsetUncertainty );
    orcaice::getPropertyAsCartesianPoint( prop, prefix+"ImuToGpsAntennaOffsetUncertainty",
                                          imuToGpsAntennaOffsetUncertainty );


    ////////////////////////////////////////
    // now writeString them out to the hardware
    char str[256];
    int put;

    //  this tells the imu where its z axis (up) is pointing. constants defined in manual.
    //  with imu mounted upside down, constant is 6 and axes are remapped: x = y, y = x, -z = z 
    sprintf( str,"setimuorientation %d\r\n", imuOrientation);
    put = serial_->writeString( str );

    // angular offset from the vehicle to the imu body. unclear how this relates to imu orientation command 
    sprintf( str,"vehiclebodyrotation %f %f %f %f %f %f\r\n",
                imuVehicleBodyRotation.x, 
                imuVehicleBodyRotation.y, 
                imuVehicleBodyRotation.z, 
                imuToGpsAntennaOffsetUncertainty.x, 
                imuToGpsAntennaOffsetUncertainty.y, 
                imuToGpsAntennaOffsetUncertainty.z );
    put = serial_->writeString( str );

    // imu to gps antenna offset
    sprintf( str,"setimutoantoffset %f %f %f %f %f %f\r\n",
                imuToGpsAntennaOffset.x, 
                imuToGpsAntennaOffset.y, 
                imuToGpsAntennaOffset.z, 
                imuToGpsAntennaOffsetUncertainty.x, 
                imuToGpsAntennaOffsetUncertainty.y, 
                imuToGpsAntennaOffsetUncertainty.z );
    put = serial_->writeString( str );


    return;
}

void
NovatelSpanInsGpsDriver::shutdown()
{
    // context_.tracer()->debug( "stopping driver", 5 );
    // hydroutil::Thread::stopAndJoin( this );
    // context_.tracer()->debug( "stopped driver", 5 );
}               

} //namespace

