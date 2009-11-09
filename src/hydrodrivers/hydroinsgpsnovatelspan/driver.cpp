/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Matthew Ridley, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <stdio.h>
#include <hydroutil/hydroutil.h>
#include <hydroiceutil/timeutils.h>
#include <hydrogpsutil/latlon2mga.h>
#include "driver.h"

#include <gbxserialacfr/gbxserialacfr.h>
#include <gbxutilacfr/gbxutilacfr.h>
#include "serialconnectivity.h"
// log types
#include <hydronovatelutil/header.h>
#include <hydronovatelutil/logtypes.h>
#include <hydronovatelutil/crc/ncrc32.h>

// novatel message definitions      
#include <hydronovatelutil/header.h>
#include <hydronovatelutil/log/rxstatus.h>
#include <hydronovatelutil/log/bestgpspos.h>
#include <hydronovatelutil/log/bestgpsvel.h>
#include <hydronovatelutil/log/inspva.h>
#include <hydronovatelutil/log/rawimu.h>

#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <vector>

using namespace std;
using namespace gbxserialacfr;
using namespace hydrogpsutil;

namespace hif = hydrointerfaces;
namespace hnu = hydronovatelutil;




namespace insgpsnovatelspan
{

// binary messages defined by novatel
#pragma pack(push,1)
    union novatel_message{
        struct{
            hnu::OEM4_BINARY_HEADER hdr;
            char               data[484];
        };
        struct{
            hnu::OEM4_SHORT_BINARY_HEADER short_hdr;
            char               short_data[500];
        };
        unsigned char raw_message[512];

        // these guys are used to directly decode messages;
        // obviously fails on endian mismatch, any sort of size mismatch and is rather nasty in general;
        // feel free to implement something better
        hnu::RXSTATUSB_LOG  rxStatus;
        hnu::BESTGPSPOS_LOG bestGpsPos;
        hnu::BESTGPSVEL_LOG bestGpsVel;
        hnu::INSPVASB_LOG   insPva;
        hnu::RAWIMUSB_LOG   rawImu;
    };
#pragma pack(pop)

Driver::Driver( const Config& cfg, const hydroutil::Context& context ) :
    swappedRollPitch_(false),
    undulation_(0),
    haveUndulation_(false),
    serial_(0),
    context_(context)
{
    hydroutil::Properties &prop = context_.properties();

    //
    // configure serial port
    baud_ = prop.getPropertyAsIntWithDefault( "Baudrate", 115200  );
    std::string serialDevice;
    if (0 != prop.getProperty( "Device", serialDevice ) ) {
        stringstream ss;
        ss << "Property was not set: "<<("Device");
        throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
    }
    serial_.reset(new Serial( serialDevice, baud_, Serial::Timeout(1,0) ));
    serial_->setDebugLevel(0);
    if(0!=connectToHardware()){
        throw (std::string("failed to connect to receiver!"));
    }
    configureImu();
    configureGps();
    requestData();
    serial_->flush();
    context_.tracer().info("Setup done, starting normal operation!");
}

Driver::~Driver() {
    // just in case something is running... stops the novatel logging any messages
    try{
        context_.tracer().info("Stopping NovatelSpan driver!");
        serial_->flush();
        serial_->writeString( "unlogall\r\n" );
        serial_->drain();
        context_.tracer().info("NovatelSpan driver stopped!");
    }
    catch(...){
        //no throwing from destructors
    }
}

int
Driver::connectToHardware() {
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
    int timeOutMsec = 150;
    std::string challenge("unlogall\r\n");
    std::string ack("<OK");
    size_t i=0;
    while(false == correctBaudrate && i<sizeof baudrates/sizeof baudrates[0]){
        currentBaudrate = baudrates[i];
        correctBaudrate = testConnectivity( challenge, ack, *(serial_.get()), timeOutMsec, maxTry, successThresh, currentBaudrate);
        i++;
    }
    if(false == correctBaudrate){
        std::cout << "!Failed to establish a connection to the receiver!\n";
        std::cout << "Check physical connections; Check manually (minicom) for Baudrates < 9600kb/s.\n";
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
    if(true == testConnectivity( challenge, ack, *(serial_.get()), timeOutMsec, maxTry, successThresh, baud_)){
        std::cout << "*******************************\n";
        return 0;
    }else{
        std::cout << "*******************************\n";
        return -1;
    }
}

void
Driver::configureImu() {
    int put;
    char str[256];

    // just in case something is running... stops the novatel logging any messages
    // serial_->flush();
    put = serial_->writeString( "unlogall\r\n" );
    //printf("put %d bytes\n",put);
    serial_->drain();

    // hardcoded settings
    //
    // tell the novatel what serial port the imu is attached to (com3 = aux)
    put = serial_->writeString( "interfacemode com3 imu imu on\r\n" );
    // the type of imu being used
    put = serial_->writeString( "setimutype imu_hg1700_ag62\r\n" );
    //force the IMU to re-align at every componentstartup
    //put = serial_->writeString( "inscommand reset\r\n" );
    //context_.tracer().info("Reset IMU; Waiting 5 seconds before continuing!");
    //sleep(5);

    ////////////////////////////////////////
    // read our params from the config file
    hydroutil::Properties &prop = context_.properties();
    std::string prefix = "Novatel.";
    //hydroutil::Properties &prop = context_.properties();
    //std::string prefix = context_.tag();
    //prefix += ".Config.Novatel.";

    // imu orientation constant
    //  this tells the imu where its z axis (up) is pointing. constants defined in manual.
    //  with imu mounted upside down, constant is 6 and axes are remapped: x = y, y = x, -z = z 
    int imuOrientation = prop.getPropertyAsIntWithDefault( prefix+"ImuOrientation", 0 );
    sprintf( str,"setimuorientation %d\r\n", imuOrientation);
    put = serial_->writeString( str );


    // vehicle to imu body rotation
    // angular offset from the vehicle to the imu body. unclear how this relates to imu orientation command 
    // the novatel docs are not especially clear on this stuff; It's highly recommended to mount the IMU
    // exactly as advised by novatel and just ignore this
    std::vector<double> imuVehicleBodyRotation(3,0.0);
    imuVehicleBodyRotation = prop.getPropertyAsDoubleVectorWithDefault(
            prefix+"VehicleBodyRotation", imuVehicleBodyRotation);

    // vehicle to imu body rotation uncertainty
    std::vector<double> imuVehicleBodyRotationUncertainty(3,0.0);
    imuVehicleBodyRotationUncertainty = prop.getPropertyAsDoubleVectorWithDefault(
            prefix+"VehicleBodyRotationUncertainty", imuVehicleBodyRotationUncertainty);
    sprintf( str,"vehiclebodyrotation %f %f %f %f %f %f\r\n",
                imuVehicleBodyRotation[0],
                imuVehicleBodyRotation[1],
                imuVehicleBodyRotation[2],
                imuVehicleBodyRotationUncertainty[0],
                imuVehicleBodyRotationUncertainty[1],
                imuVehicleBodyRotationUncertainty[2] );
    put = serial_->writeString( str );


    // imuToGpsAntennaOffset
    // The span system kalman fiter needs this info; make _sure_ you do this right
    std::vector<double> imuToGpsAntennaOffset(3,0.0);
    prop.getPropertyAsDoubleVectorWithDefault( prefix+"ImuToGpsAntennaOffset", imuToGpsAntennaOffset );

    // imuToGpsAntennaOffsetUncertainty
    std::vector<double> imuToGpsAntennaOffsetUncertainty(3,0.0);
    prop.getPropertyAsDoubleVectorWithDefault( prefix+"ImuToGpsAntennaOffsetUncertainty",
                                          imuToGpsAntennaOffsetUncertainty );
    sprintf( str,"setimutoantoffset %f %f %f %f %f %f\r\n",
                imuToGpsAntennaOffset[0], 
                imuToGpsAntennaOffset[1], 
                imuToGpsAntennaOffset[2], 
                imuToGpsAntennaOffsetUncertainty[0], 
                imuToGpsAntennaOffsetUncertainty[1], 
                imuToGpsAntennaOffsetUncertainty[2] );
    put = serial_->writeString( str );


    // roll pitch correction
    // internal hack to get around the problems of setting the imu orientation correctly
    int enableMode = prop.getPropertyAsIntWithDefault( prefix+"swappedRollPitch", 0 );
    if(enableMode){
        context_.tracer().info("Turning ON roll/pitch swap!");
        context_.tracer().warning("!!! Swapping roll and pitch in ins and respective rates in imu !!!");
        swappedRollPitch_=true;
    }else{
        context_.tracer().info("Turning OFF roll/pitch swap!");
        context_.tracer().warning("!!! NOT swapping roll/pitch !!!");
        swappedRollPitch_=false;
    }

    return;
}

void
Driver::configureGps() {
    // hardcoded settings first

    // for dgps
    // mm jan/08: not sure why these are commented out, looks similar to the rtk stuff further down;
    //            maybe for disabling it??
    // put = serial_->writeString( "com com2 57600 n 8 1 n off on\r\n" );
    // put = serial_->writeString( "interfacemode com2 rtca none off\r\n" );

    // turn off posave as this command implements position averaging for base stations.
    int put = serial_->writeString( "posave off\r\n" );
    // make sure that fixposition has not been set
    put = serial_->writeString( "fix none\r\n" );
    // select the geodetic datum for operation of the receiver (wgs84 = default)
    put = serial_->writeString( "datum wgs84\r\n" );

    // read mode params from the config file
    hydroutil::Properties &prop = context_.properties();
    std::string prefix("Novatel.Mode.");
    int enableMode;

    // CDGPS
    enableMode = prop.getPropertyAsIntWithDefault( prefix+"CDGPS", 0 );
    if(enableMode){
        context_.tracer().info("Turning on CDGPS!");
        put = serial_->writeString( "ASSIGNLBAND CDGPS 1547547 4800\r\n" );
    }

    //!!!! NOT active at the moment; might invalidate our OmniSTAR setup !!!!!

    // mm jan/08: I'm not sure if it's safe to (re-)set this every time the driver is started.
    // It is definitely safe to set this manually when you get your subscription and do
    // a safeconfig command once you have it working.
    // However, a freset command (for whatever reason) will invalidate that.

    //OmniSTAR
    //enableMode = prop.getPropertyAsIntWithDefault( modeProp, modePrefix+"OmniSTAR", 0 );
    //if(enableMode){
        //cout << "turning on OmniSTAR!" << endl;
        //put = serial_->writeString( "ASSIGNLBAND OMNISTAR 1536782 1200\r\n" );
    //}

    //!!!! NOT active at the moment; might invalidate our OmniSTAR setup !!!!!

    // rtk
    enableMode = prop.getPropertyAsIntWithDefault( prefix+"rtk", 0 );
    if(enableMode){
        context_.tracer().info("Turning on RTK!");
        put = serial_->writeString( "com com2,9600,n,8,1,n,off,on\r\n" );
        put = serial_->writeString( "interfacemode com2 rtca none\r\n" );
    }

    // turn SBAS on/off (essentially global DGPS)
    enableMode = prop.getPropertyAsIntWithDefault( prefix+"SBAS", 0 );
    if(enableMode){
        context_.tracer().info("Turning on SBAS!");
        put = serial_->writeString( "SBASCONTROL ENABLE Auto 0 ZEROTOTWO\r\n");
        //we try to use WAAS satellites even below the horizon
        put = serial_->writeString( "WAASECUTOFF -5.0\r\n");
    }
    else{
        context_.tracer().info("Turning off SBAS!");
        put = serial_->writeString( "SBASCONTROL DISABLE Auto 0 NONE\r\n");
    }

    //Let the receiver figure out which rtk corrections are best
    //put = serial_->writeString( "RTKSOURCE AUTO\r\n" );

    //We only use our own rtk corrections; _not_ OmniSTAR HP
    put = serial_->writeString( "RTKSOURCE RTCA ANY\r\n" );
    //Let the receiver figure out which range corrections are best
    put = serial_->writeString( "PSRDIFFSOURCE AUTO\r\n" );

}

void
Driver::requestData() {
    hydroutil::Properties &prop = context_.properties();
    std::string prefix("Novatel.LogEnable.");
    int enableMsg;
    int put;

    // GPS messages

    // receiver status
    enableMsg = prop.getPropertyAsIntWithDefault( prefix+"rxstatusb", 0 );
    if(enableMsg){
        context_.tracer().info("turning on receiver status!");
        put = serial_->writeString( "log rxstatusb onchanged\r\n" );
    }

    // gps position without ins
    enableMsg = prop.getPropertyAsIntWithDefault( prefix+"bestgpsposb", 0 );
    if(enableMsg){
        context_.tracer().info("Turning on GPS position at 5Hz!");
        put = serial_->writeString( "log bestgpsposb ontime 0.2\r\n" );
    }


    // INS messages

    // pva data in wgs84 coordinates
    enableMsg = prop.getPropertyAsIntWithDefault( prefix+"inspvasb", 1 );
    if(enableMsg){
        context_.tracer().info("Turning on INS position/velocity/oriantation at 100Hz!");
        put = serial_->writeString( "log inspvasb ontime 0.01\r\n" );
    }


    // IMU messages

    // raw accelerometer and gyro data
    enableMsg = prop.getPropertyAsIntWithDefault( prefix+"rawimusb", 0 );
    if(enableMsg){
        sleep(1);
        enableMsg = prop.getPropertyAsIntWithDefault( prefix+"inspvasb", 1 );
        if(enableMsg){
            context_.tracer().info("!!We want raw IMU as well!!\n;Turning on INS position/velocity/oriantation at 50Hz only!");
            put = serial_->writeString( "log inspvasb ontime 0.02\r\n" );
        }
        context_.tracer().info("Turning on raw imu data!");
        put = serial_->writeString( "log rawimusb onnew\r\n" );
    }

    return;
}

std::auto_ptr<hydrointerfaces::InsGps::GenericData>
Driver::read(){
    union novatel_message msg;
    std::auto_ptr<hydrointerfaces::InsGps::GenericData> data;
    data.reset(0);

    // temporary storage to assemble GpsData from velocity and position
    hnu::BESTGPSPOS_LOG bestGpsPosTmp;
    hnu::BESTGPSVEL_LOG bestGpsVelTmp;
    bool haveGpsPos = false;
    bool haveGpsVel = false;
    struct timeval timeOfGpsPos = {0,0};
    struct timeval timeOfGpsVel = {0,0};
    double dtGps;

    // read msg from hardware
    do{
        // Timeouts are not adjusted once a serial call returns;
        // So we could be stuck here for longer than the set timeout.
        int ret;
        ret = serial_->bytesAvailableWait();
        if ( ret >= 0 ) {
            switch(readNovatelMessage(msg)){
                case hnu::RXSTATUSB_LOG_TYPE:
                    //data = novatel2Hydro(msg.rxStatus);
                    //not sure how to handle this properly, do we need a separat channel,
                    //or do we remember useful stuff and report it with the next data packet??
                    //If this guy is reported onchange, it might imply something serious and
                    //warrant an exception.
                    //
                    //Ignore for now
                    {
                        std::stringstream ss;
                        ss << "Warning("<<__FILE__<<":"<< __LINE__
                            <<" : got RXSTATUS from receiver; Not handled at the moment" <<std::endl;
                        context_.tracer().warning( ss.str() );
                    }
                    break;
                case hnu::INSPVASB_LOG_TYPE:
                    data = novatel2Hydro(msg.insPva);
                    break;
                case hnu::BESTGPSVELB_LOG_TYPE:
                    bestGpsVelTmp = msg.bestGpsVel;
                    timeOfGpsVel = timeOfRead_;
                    haveGpsVel = true;
                    // if we have both velocity and position, we can assemble it and send it away
                    dtGps = fabs (hydroiceutil::timeAsDouble(timeOfGpsPos.tv_sec, timeOfGpsPos.tv_usec) - hydroiceutil::timeAsDouble(timeOfGpsVel.tv_sec, timeOfGpsVel.tv_usec));
                    if(haveGpsPos && 0.04 > dtGps ){
                        data = novatel2Hydro(bestGpsPosTmp, bestGpsVelTmp);
                        haveGpsVel = false;
                        haveGpsPos = false;
                    }
                    break;
                case hnu::BESTGPSPOSB_LOG_TYPE:
                    bestGpsPosTmp = msg.bestGpsPos;
                    timeOfGpsPos = timeOfRead_;
                    haveGpsPos = true;
                    // if we have both velocity and position, we can assemble it and send it away
                    dtGps = fabs (hydroiceutil::timeAsDouble(timeOfGpsPos.tv_sec, timeOfGpsPos.tv_usec) - hydroiceutil::timeAsDouble(timeOfGpsVel.tv_sec, timeOfGpsVel.tv_usec));
                    if(haveGpsVel && 0.04 > dtGps ){
                        data = novatel2Hydro(bestGpsPosTmp, bestGpsVelTmp);
                        haveGpsVel = false;
                        haveGpsPos = false;
                    }
                    // need to remember undulation, so that INS can supply height above MSL
                    undulation_ = msg.bestGpsPos.data.undulation;
                    haveUndulation_ = true;
                    break;
                case hnu::RAWIMUSB_LOG_TYPE:
                    data = novatel2Hydro(msg.rawImu);
                    break;
                default:
                    {
                        std::stringstream ss;
                        ss << "Warning("<<__FILE__<<":"<< __LINE__
                            <<" : got unexpected message from receiver; id: " << msg.hdr.number << std::endl;
                        throw( ss.str() );
                    }
                    break;
            }
        }
        else {
            std::stringstream ss;
            ss << "Warning("<<__FILE__<<":"<< __LINE__
             << "Timed out while waiting for data";
            throw (ss.str());
        }
    }while(NULL == data.get()); // repeat till we get valid data

    return data;
}

int
Driver::readNovatelMessage(union novatel_message &msg ) {
    // read the three sync bytes which are always at the start of the message header
    unsigned short id;
    unsigned long crc;
    unsigned long in_crc;
    msg.hdr.sop1 = 0;
    int skip = -1;
    int got;

    // read the first sync byte
    do{
        got = serial_->readFull( &msg.hdr.sop1, 1 );
        if ( got <= 0 ) {
            return got;
        }
        if( got>0 ) {
            skip++;
        }
    }while( msg.hdr.sop1 != 0xaa );

    // get timestamp after the first byte for accuracy
    {
        int sec;
        int usec;
        hydroiceutil::now(sec, usec);
        timeOfRead_.tv_sec = sec;
        timeOfRead_.tv_usec = usec;
    }

    if( skip>0 ) {
        //printf("skipped %d bytes\n",skip);
    }

    // read the second sync byte
    do {
        got = serial_->readFull( &msg.hdr.sop2, 1 );
        if ( got <= 0 ) {
            return got;
        }
    }while( got!=1 );

    if( msg.hdr.sop2 != 0x44 ) {
        return -1;
        //try again ??
    }

     // read the third sync byte
    do {
        got = serial_->readFull( &msg.hdr.sop3, 1 );
        if ( got <= 0 ) {
            return got;
        }
    }while( got != 1 );

    switch( msg.hdr.sop3 ) {
        case 0x12: //long packet
            if( // how long is the header ?
                -1 == serial_->readFull( &msg.hdr.header_length, 1 )
                // read all of the header...
                || -1 == serial_->readFull( &msg.hdr.number, msg.hdr.header_length-4 )
                // read the  message data
                || -1 == serial_->readFull( &msg.data, msg.hdr.length )
                || -1 == serial_->readFull( &in_crc, 4 )
              ){
                return -1;
            }

            id = msg.hdr.number;

            crc = hnu::CalculateCRC32( msg.raw_message,
                               msg.hdr.length+msg.hdr.header_length );
            break;

        case 0x13: //short packet
            if( // read rest of the header 12 bytes - 3 bytes already read, then the actual data, then the CRC
                -1 == serial_->readFull( &msg.short_hdr.length, 9 )
                || -1 == serial_->readFull( &msg.short_data, msg.short_hdr.length )
                || -1 == serial_->readFull( &in_crc, 4 )
              ){
                return -1;
            }

            id = msg.short_hdr.number;

            crc = hnu::CalculateCRC32( msg.raw_message,msg.short_hdr.length + 12 );
            break;

        default: //bollocks
            return -1;
            break;
    }

    if(in_crc != crc) {
        fprintf( stderr,"CRC Error: 0x%lx, 0x%lx\n",in_crc,crc );
        throw std::string( "CRC Error" );
        return -1;
    }

    return  id;
}


std::auto_ptr<hif::InsGps::GenericData>
Driver::novatel2Hydro(hnu::INSPVASB_LOG &insPva){
    hif::InsGps::InsData *data = new hif::InsGps::InsData;
    std::auto_ptr<hif::InsGps::GenericData> genericData( data );
    data->lat = insPva.data.longitude;
    data->lon = insPva.data.latitude;
    data->alt = insPva.data.height;
    data->altAMSL = haveUndulation_ ? (insPva.data.height - undulation_) : nan("NaN"); //I _think_ that a NaN  is a better choice than lying in some way (i.e. set to zero, height, 1234, ...), but I'm open to different arguments
    data->vENU[0] = insPva.data.east_vel;
    data->vENU[1] = insPva.data.north_vel;
    data->vENU[2] = insPva.data.up_vel;
    if(true == swappedRollPitch_){
        data->oRPY[0] = insPva.data.pitch/180*M_PI;
        data->oRPY[1] = insPva.data.roll/180*M_PI;
    }else{
        data->oRPY[0] = insPva.data.roll/180*M_PI;
        data->oRPY[1] = insPva.data.pitch/180*M_PI;
    }
    data->oRPY[2] = 2*M_PI - insPva.data.yaw/180*M_PI; //TODO: Check this (0 == true/magnetic/grid North, East?)

    data->time.tv_sec = timeOfRead_.tv_sec;
    data->time.tv_usec = timeOfRead_.tv_usec;

    switch( insPva.data.status ) {
        case 0:
            data->statusMessage = "Ins Status: Ins is inactive";
            data->statusMessageType = Fault;
            break;
        case 1:
            data->statusMessage = "Ins Status: Ins is aligning";
            data->statusMessageType = Warning;
            break;
        case 2:
            data->statusMessage = "Ins Status: Ins solution is bad";
            data->statusMessageType = Warning;
            break;
        case 3:
            data->statusMessage = "Ins Status: Ins solution is good";
            data->statusMessageType = Ok;
            break;
        case 4://fallthrough
        case 5:
            {
                stringstream ss;
                ss << "Ins Status: reserved value?? Check NovatelSpan manual for \"" << insPva.data.status << "\" as INS status";
                data->statusMessage = ss.str();
                data->statusMessageType = Warning;
            }
            break;
        case 6:
            data->statusMessage = "Ins Status: Bad Ins Gps agreement";
            data->statusMessageType = Warning;
            break;
        case 7:
            data->statusMessage = "Ins Status: Ins alignment is complete but vehicle must perform maneuvers so that the attitude can converge";
            data->statusMessageType = Ok;
            break;
        default:
            {
                stringstream ss;
                ss <<  "Ins Status: Unknown Ins Status. Check NovatelSpan manual for \"" << insPva.data.status << "\" as INS status";
                data->statusMessage = ss.str();
                data->statusMessageType = Warning;
            }
            break;
    }

    return genericData;

}

std::auto_ptr<hif::InsGps::GenericData>
Driver::novatel2Hydro(hnu::BESTGPSPOS_LOG &bestGpsPos, hnu::BESTGPSVEL_LOG &bestGpsVel){
    static int cnt;
    if(0 == cnt++ % 100) cout << __func__ << " gps; implement me properly!\n";
    hif::InsGps::GpsData *data = new hif::InsGps::GpsData;
    std::auto_ptr<hif::InsGps::GenericData> genericData( data );

    data->lat = bestGpsPos.data.latitude;
    data->lon = bestGpsPos.data.longitude;
    data->alt = bestGpsPos.data.height;
    data->altAMSL = bestGpsPos.data.height-bestGpsPos.data.undulation;
    data->sigmaLat = bestGpsPos.data.lat_sigma; //check units
    data->sigmaLon = bestGpsPos.data.lon_sigma; //check units
    data->sigmaAlt = bestGpsPos.data.hgt_sigma;

    data->heading = bestGpsVel.data.heading;
    data->speed = bestGpsVel.data.hor_speed;
    data->climbRate = bestGpsVel.data.vert_speed;
    data->latency.push_back(bestGpsVel.data.latency);

    data->sat = bestGpsPos.data.num_GPSL1;
    data->obsL1 = bestGpsPos.data.num_L1;
    data->obsL2 = bestGpsPos.data.num_L2;

    data->time.tv_sec = timeOfRead_.tv_sec;
    data->time.tv_usec = timeOfRead_.tv_usec;

    double dt = (bestGpsPos.hdr.millisecs - bestGpsVel.hdr.millisecs);
    if(fabs(dt) > 1.0){
        stringstream ss;
        ss << "GPS position/velocity time stamp mismatch! dt: " << dt << "ms";
        context_.tracer().warning(ss.str());
    }
    if(bestGpsPos.data.pos_type != bestGpsVel.data.vel_type){
        stringstream ss;
        ss << "GPS position/velocity type mismatch! pos_type: " <<bestGpsPos.data.pos_type<<", vel_type: " <<bestGpsVel.data.vel_type;
        context_.tracer().warning(ss.str());
    }
    if(bestGpsPos.data.sol_status != bestGpsVel.data.sol_status){
        stringstream ss;
        ss << "GPS position/velocity solution status mismatch! sol_status (pos): " <<bestGpsPos.data.sol_status<<", sol_status (vel): " <<bestGpsVel.data.sol_status;
        context_.tracer().warning(ss.str());
    }

    switch(bestGpsPos.data.pos_type){
        case hnu::NONE:
        case hnu::INS: // yep, this counts as NoFix; the numbers are probably ok, but they come from the INS, _not_ a GPS fix
            data->gpsSolutionStatus = NoFix;
            data->gpsCorrectionMethod = NoDiff;
            break;

        case hnu::FIXEDPOS:
        case hnu::FIXEDHEIGHT:
            //debatable; In a (mobile) robotics context I'd label it bad.
            data->gpsSolutionStatus = BadFix;
            data->gpsCorrectionMethod = NoDiff;
            break;

        case hnu::SINGLE:
        case hnu::DOPPLER_VELOCITY:
        case hnu::INS_PSRSP:
            data->gpsSolutionStatus = GoodFix;
            data->gpsCorrectionMethod = NoDiff;
            break;

        case hnu::PSRDIFF:
        case hnu::INS_PSRDIFF:
            data->gpsSolutionStatus = GoodFix;
            data->gpsCorrectionMethod = CodeDiff;
            data->gpsCorrectionSource.resize(1);
            data->gpsCorrectionSource[0] = Local;
            break;

        case hnu::WAAS:
        case hnu::OMNISTAR:
        case hnu::CDGPS:
            data->gpsSolutionStatus = GoodFix;
            data->gpsCorrectionMethod = CodeDiff;
            data->gpsCorrectionSource.resize(1);
            data->gpsCorrectionSource[0] = Satellite;
            break;

        case hnu::FLOATCONV:
        case hnu::WIDELANE:
        case hnu::NARROWLANE:
        case hnu::PROPOGATED:
        case hnu::L1_FLOAT:
        case hnu::IONFREE_FLOAT:
        case hnu::NARROW_FLOAT:
        case hnu::L1_INT:
        case hnu::WIDE_INT:
        case hnu::NARROW_INT:
        case hnu::RTK_DIRECT_INS:
        case hnu::INS_RTKFLOAT:
        case hnu::INS_RTKFIXED:
            data->gpsSolutionStatus = GoodFix;
            data->gpsCorrectionMethod = CarrierDiff;
            data->gpsCorrectionSource.resize(1);
            data->gpsCorrectionSource[0] = Local;
            break;

        case hnu::OMNISTAR_HP:
        case hnu::OMNISTAR_XP:
            data->gpsSolutionStatus = GoodFix;
            data->gpsCorrectionMethod = CarrierDiff;
            data->gpsCorrectionSource.resize(1);
            data->gpsCorrectionSource[0] = Satellite;
            break;

        default:
            data->gpsSolutionStatus = NoFix;
            data->gpsCorrectionMethod = NoDiff;
            break;

    }



    switch(bestGpsPos.data.sol_status){
        case hnu::SOL_COMPUTED:
            data->gpsSolutionStatus = GoodFix; // really??
            break;

        case hnu::COLD_START:
        case hnu::V_H_LIMIT:
        case hnu::INSUFFICIENT_OBS:
            data->gpsSolutionStatus = NoFix;
            break;

        case hnu::NO_CONVERGENCE:
        case hnu::SINGULARITY:
        case hnu::COV_TRACE:
        case hnu::TEST_DIST:
        case hnu::VARIANCE:
        case hnu::RESIDUALS:
        case hnu::DELTA_POS:
        case hnu::NEGATIVE_VAR:
        case hnu::INTEGRITY_WARNING:
        case hnu::PENDING:
        case hnu::INVALID_FIX:
            //if anything looks fishy, we label it as bad
            data->gpsSolutionStatus = BadFix;
            break;

        case hnu::INS_INACTIVE:
        case hnu::INS_ALIGNING:
        case hnu::INS_BAD:
        case hnu::IMU_UNPLUGGED: //these guys have fuck all to do with GPS, but are intermixed here anyhow
        default:
            data->gpsSolutionStatus = NoFix; //if in doubt we ignore it; might have to rethink if it occurs often
            {
                stringstream ss;
                ss << "sol_status unknown or not GPS related: " << bestGpsPos.data.sol_status << "setting gpsSolutionStatus to NoFix";
                context_.tracer().warning(ss.str());
            }
            break;
    }
    if(NoDiff != data->gpsCorrectionMethod){
        data->gpsCorrectionAge.resize(1);
        data->gpsCorrectionAge[0] = bestGpsPos.data.diff_age;
        data->gpsBaseStationId.assign(bestGpsPos.data.station_id,4);
    }
    return genericData;
}

std::auto_ptr<hif::InsGps::GenericData>
Driver::novatel2Hydro(hnu::RAWIMUSB_LOG &rawImu){
    static int cnt;
    if(0 == cnt++ % 500) cout << __func__ << " imu; implement me properly!\n";
    hif::InsGps::ImuData *data = new hif::InsGps::ImuData;
    std::auto_ptr<hif::InsGps::GenericData> genericData( data );

    // Note scale factors and axis translation
    // TODO: are these gyro values correct? Does this yield rad/sec?
    cout << __func__ << " check conversion factors!\n";
    double dt = 0.01;
    // Divide by dt to get the correct units for the IMU data
    if(true == swappedRollPitch_){
        data->turnRate[0] = hnu::IMU_GYRO_CONST * (double)rawImu.data.x_gyro/dt;
        data->turnRate[1] = -hnu::IMU_GYRO_CONST * (double)rawImu.data.y_gyro/dt;
    }else{
        data->turnRate[0] = -hnu::IMU_GYRO_CONST * (double)rawImu.data.y_gyro/dt;
        data->turnRate[1] = hnu::IMU_GYRO_CONST * (double)rawImu.data.x_gyro/dt;
    }
    data->turnRate[2] = -hnu::IMU_GYRO_CONST * (double)rawImu.data.z_gyro/dt;

    data->acc[0] = hnu::IMU_ACCEL_CONST * (double)rawImu.data.y_accel/dt;
    data->acc[1] = -hnu::IMU_ACCEL_CONST * (double)rawImu.data.x_accel/dt;
    data->acc[2] = hnu::IMU_ACCEL_CONST * (double)rawImu.data.z_accel/dt;
    data->biasCorrected = false; //really ?? check
    data->tempr.resize(1);
    data->tempr[0] = static_cast<double>((rawImu.data.status & hnu::IMU_STATUS_TEMP)>>8);

    data->time.tv_sec = timeOfRead_.tv_sec;
    data->time.tv_usec = timeOfRead_.tv_usec;

    return genericData;
}


} //namespace

extern "C" {
    hif::InsGpsFactory *createInsGpsDriverFactory()
    { return new insgpsnovatelspan::Factory; }
}
