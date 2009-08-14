/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Michael Moser
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <vector>

#include <hydroutil/hydroutil.h>
#include <hydroiceutil/timeutils.h>
//#include <hydrogpsutil/latlon2mga.h>

#include <hydrointerfaces/insgps.h>

#include <gbxutilacfr/trivialtracer.h>
#include <gbxutilacfr/exceptions.h>
#include <gbxnovatelacfr/driver.h>

#include "driver.h"


namespace hif = hydrointerfaces;
namespace gna = gbxnovatelacfr;
namespace gua = gbxutilacfr;

namespace{
//conversion functions
std::auto_ptr<hif::InsGps::GenericData>
gbx2Hydro(gna::InsPvaData *insPva, bool haveUndulation, double undulation);
std::auto_ptr<hif::InsGps::GenericData>
gbx2Hydro(gna::BestGpsPosData *bestGpsPos, gna::BestGpsVelData *bestGpsVel);
std::auto_ptr<hif::InsGps::GenericData>
gbx2Hydro(gna::RawImuData *rawImu);
void gbx2Hydro( enum hif::InsGps::StatusMessageType &hydroStatusType,
        std::string &hydroStatusMessage,
        enum gna::StatusMessageType &gbxStatusType,
        std::string &gbxStatusMessage);
}



namespace insgpsnovatelgbx
{

Driver::Driver( const Config& config, const hydroutil::Context& context ) :
    undulation_(nan("NaN")),
    //haveUndulation_(false),
    device_(0),
    context_(context)
{
    haveGpsPos_ = false;
    haveGpsVel_ = false;
    timeOfGpsPos_.tv_sec = 0;
    timeOfGpsPos_.tv_usec = 0;
    timeOfGpsVel_.tv_sec = 0;
    timeOfGpsVel_.tv_usec = 0;

    hydroutil::Properties &prop = context_.properties();
    std::string prefix("Novatel.");
    //use sensible defaults
    std::string serialDevice = "";
    int baud = 115200;
    std::string imuType = "";


    // Get Parameters for SimpleConfig

    // configure serial port
    baud = prop.getPropertyAsIntWithDefault( prefix+"Baudrate", baud  );
    if (0 != prop.getProperty( "Device", serialDevice ) ) {
        std::stringstream ss;
        ss << "Required property was not set: "<<(prefix+"Device");
        throw gua::Exception( ERROR_INFO, ss.str() );
    }
    if (0 != prop.getProperty( "ImuType", imuType ) ) {
        std::stringstream ss;
        ss << "Required property was not set: "<<(prefix+"ImuType");
        throw gua::Exception( ERROR_INFO, ss.str() );
    }
    // imuToGpsAntennaOffset
    // The span system kalman fiter needs this info; make _sure_ you do this right
    std::vector<double> imuToGpsOffset;
    if(0 != prop.getPropertyAsDoubleVector( prefix+"ImuToGpsOffset", imuToGpsOffset )){
        std::stringstream ss;
        ss << "Required property was not set: "<<(prefix+"ImuToGpsOffset");
        throw gua::Exception( ERROR_INFO, ss.str() );
    }

    // produce a sensible configuration with the info so far
    gna::SimpleConfig simpleCfg(serialDevice, baud, imuType, imuToGpsOffset);
    gna::Config cfg(simpleCfg);


    // Refine configuration

    prop.getPropertyAsDoubleVectorWithDefault( prefix+"ImuToGpsOffsetUncertainty", cfg.imuToGpsOffsetUncertainty_ );

    // imu orientation constant
    //  this tells the imu where its z axis (up) is pointing. constants defined in manual.
    //  with imu mounted upside down, constant is 6 and axes are remapped: x = y, y = x, -z = z 
    cfg.enableSetImuOrientation_ = prop.getPropertyAsIntWithDefault( prefix+"EnableSetImuOrientation", 0 );
    if(true == cfg.enableSetImuOrientation_){
        cfg.setImuOrientation_ = prop.getPropertyAsIntWithDefault( prefix+"SetImuOrientation", 0 );
    }

    // vehicle to imu body rotation
    // angular offset from the vehicle to the imu body. unclear how this relates to imu orientation command 
    // the novatel docs are not especially clear on this stuff; It's highly recommended to mount the IMU
    // exactly as advised by novatel and just ignore this
    cfg.enableVehicleBodyRotation_ = prop.getPropertyAsIntWithDefault( prefix+"EnableVehicleBodyRotation", 0 );
    if(true == cfg.enableVehicleBodyRotation_){
        prop.getPropertyAsDoubleVectorWithDefault( prefix+"VehicleBodyRotation", cfg.vehicleBodyRotation_ );
        prop.getPropertyAsDoubleVectorWithDefault( prefix+"VehicleBodyRotationUncertainty", cfg.vehicleBodyRotationUncertainty_ );
    }

    // report INS pos/vel offset from the IMU
    cfg.enableInsOffset_ = prop.getPropertyAsIntWithDefault( prefix+"EnableInsOffset", 0 );
    if(true == cfg.enableVehicleBodyRotation_){
        prop.getPropertyAsDoubleVectorWithDefault( prefix+"InsOffset", cfg.insOffset_ );
    }

    //tightly coupled (phase based vs position based) filter; Chance of better performance in adverse conditions
    cfg.enableInsPhaseUpdate_ = prop.getPropertyAsIntWithDefault( prefix+"EnableInsPhaseUpdate", 0 );

    //GPS corrections
    cfg.enableCDGPS_ = prop.getPropertyAsIntWithDefault( prefix+"EnableCDGPS", 0 );
    cfg.enableSBAS_ = prop.getPropertyAsIntWithDefault( prefix+"EnableSBAS", 0 );
    cfg.enableRTK_ = prop.getPropertyAsIntWithDefault( prefix+"EnableRTK", 0 );
    cfg.enableUseOfOmniStarCarrier_ = prop.getPropertyAsIntWithDefault( prefix+"EnableUseOfOmniStarCarrier", 0 );

    int tmp;
    if((0 != prop.getPropertyAsInt( prefix+"EnableInsPva", tmp)) && (tmp ==1)){
        cfg.enableInsPva_ = tmp;
        cfg.dtInsPva_ = prop.getPropertyAsDoubleWithDefault( prefix+"DtInsPva", cfg.dtInsPva_);
    }
    if((0 != prop.getPropertyAsInt( prefix+"EnableGpsPos", tmp)) && (tmp ==1)){
        cfg.enableGpsPos_ = tmp;
        cfg.dtGpsPos_ = prop.getPropertyAsDoubleWithDefault( prefix+"DtGpsPos", cfg.dtGpsPos_);
    }
    if((0 != prop.getPropertyAsInt( prefix+"EnableGpsVel", tmp)) && (tmp ==1)){
        cfg.enableGpsVel_ = tmp;
        cfg.dtGpsVel_ = prop.getPropertyAsDoubleWithDefault( prefix+"DtGpsVel", cfg.dtGpsVel_);
    }
    if((0 != prop.getPropertyAsInt( prefix+"EnableRawImu", tmp)) && (tmp ==1)){
        cfg.enableRawImu_ = tmp;
    }

    context_.tracer().info("Got Configuration Options, creating Device!");
    device_.reset(new gbxnovatelacfr::Driver(cfg, context_.tracer()));
    context_.tracer().info("Setup done, starting normal operation!");
}

std::auto_ptr<hydrointerfaces::InsGps::GenericData>
Driver::read(){
    std::auto_ptr<hydrointerfaces::InsGps::GenericData> data;  // outgoing data
    std::auto_ptr<gna::GenericData > gbxGeneric;  // incoming data from device



    // read msg from hardware, then figure out what we've got and process it
    // need to do it in a loop in case we get a multi-part message that needs to be assembled (gps)
    do{
        gbxGeneric = device_->read(); // this guy might throw, but we pass this up; Otherwise we would need to make sure to not block indefinitely
        switch(gbxGeneric->type()){
            case gna::InsPva:
                {
                    gna::InsPvaData *gbxData = dynamic_cast<gna::InsPvaData *>(gbxGeneric.get());
                    assert(0 != gbxData);
                    data = gbx2Hydro(gbxData, haveUndulation_, undulation_);
                }
                break;
            case gna::BestGpsPos:
                {
                    bestGpsPosTmp_.reset(dynamic_cast<gna::BestGpsPosData *>(gbxGeneric.release())); // we need to keep this guy around
                    assert(0 != bestGpsPosTmp_.get());
                    timeOfGpsPos_.tv_sec = bestGpsPosTmp_->timeStampSec;
                    timeOfGpsPos_.tv_usec = bestGpsPosTmp_->timeStampUSec;
                    haveGpsPos_ = true;
                    // need to remember undulation, so that INS can supply height above MSL
                    undulation_ = bestGpsPosTmp_->undulation;
                    haveUndulation_ = true;
                    // if we have both velocity and position, we can assemble it and send it away
                    double dtGps = fabs(
                        hydroiceutil::timeAsDouble(timeOfGpsPos_.tv_sec, timeOfGpsPos_.tv_usec) -
                        hydroiceutil::timeAsDouble(timeOfGpsVel_.tv_sec, timeOfGpsVel_.tv_usec) );
                    if(haveGpsVel_ && 0.04 > dtGps ){
                        data = gbx2Hydro(bestGpsPosTmp_.get(), bestGpsVelTmp_.get());
                        haveGpsVel_ = false;
                        haveGpsPos_ = false;
                    }
                }
                break;
            case gna::BestGpsVel:
                {
                    bestGpsVelTmp_.reset(dynamic_cast<gna::BestGpsVelData *>(gbxGeneric.get()));
                    assert(0 != bestGpsVelTmp_.get());
                    timeOfGpsVel_.tv_sec = bestGpsVelTmp_->timeStampSec;
                    timeOfGpsVel_.tv_usec = bestGpsVelTmp_->timeStampUSec;
                    haveGpsVel_ = true;
                    // if we have both velocity and position, we can assemble it and send it away
                    double dtGps = fabs (hydroiceutil::timeAsDouble(timeOfGpsPos_.tv_sec, timeOfGpsPos_.tv_usec) - hydroiceutil::timeAsDouble(timeOfGpsVel_.tv_sec, timeOfGpsVel_.tv_usec));
                    if(haveGpsPos_ && 0.04 > dtGps ){
                        data = gbx2Hydro(bestGpsPosTmp_.get(), bestGpsVelTmp_.get());
                        haveGpsVel_ = false;
                        haveGpsPos_ = false;
                    }
                }
                break;
            case gna::RawImu:
                {
                    gna::RawImuData *gbxData = dynamic_cast<gna::RawImuData *>(gbxGeneric.get());
                    assert(0 != gbxData);
                    data = gbx2Hydro(gbxData);
                }
                break;
            default:
                {
                    std::stringstream ss;
                    ss << "Got unknown message; Type: "<< gbxGeneric->type() << "; Content: " << gbxGeneric->toString();
                    throw (gua::Exception(ERROR_INFO, ss.str()));
                }
                break;
        }
    }while(0 == data.get());

    return data;
}
}//namespace

namespace{
//conversion functions

std::auto_ptr<hif::InsGps::GenericData>
gbx2Hydro(gna::InsPvaData *insPva, bool haveUndulation, double undulation){
    hif::InsGps::InsData *data = new hif::InsGps::InsData;
    std::auto_ptr<hif::InsGps::GenericData> genericData( data );
    data->lat = insPva->latitude;
    data->lon = insPva->longitude;
    data->alt = insPva->height;
    //data->altAMSL = haveUndulation ? (insPva->height - undulation) : nan("NaN"); //I _think_ that a NaN  is a better choice than lying in some way (i.e. set to zero, height, 1234, ...), but I'm open to different arguments
    data->altAMSL = insPva->height - undulation;
    data->vENU[0] = insPva->eastVelocity;
    data->vENU[1] = insPva->northVelocity;
    data->vENU[2] = insPva->upVelocity;
    data->oRPY[0] = insPva->roll/180*M_PI;
    data->oRPY[1] = insPva->pitch/180*M_PI;
    data->oRPY[2] = 2*M_PI - insPva->azimuth/180*M_PI;

    data->time.tv_sec = insPva->timeStampSec;
    data->time.tv_usec = insPva->timeStampUSec;

    gbx2Hydro(data->statusMessageType, data->statusMessage, insPva->statusMessageType, insPva->statusMessage);

    return genericData;

}

std::auto_ptr<hif::InsGps::GenericData>
gbx2Hydro(gna::BestGpsPosData *bestGpsPos, gna::BestGpsVelData *bestGpsVel){
    static int cnt;
    if(0 == cnt++ % 100) std::cout << __func__ << " gps; implement me properly!\n";
    hif::InsGps::GpsData *data = new hif::InsGps::GpsData;
    std::auto_ptr<hif::InsGps::GenericData> genericData( data );

    data->lat = bestGpsPos->latitude;
    data->lon = bestGpsPos->longitude;
    data->alt = bestGpsPos->heightAMSL + bestGpsPos->undulation;
    data->altAMSL = bestGpsPos->heightAMSL;
    data->sigmaLat = bestGpsPos->sigmaLatitude;
    data->sigmaLon = bestGpsPos->sigmaLongitude;
    data->sigmaAlt = bestGpsPos->sigmaHeight;

    data->heading = bestGpsVel->trackOverGround;
    data->speed = bestGpsVel->horizontalSpeed;
    data->climbRate = bestGpsVel->verticalSpeed;
    data->latency.push_back(bestGpsVel->latency);

    data->sat = bestGpsPos->numL1Ranges;
    data->obsL1 = bestGpsPos->numL1RangesRTK;
    data->obsL2 = bestGpsPos->numL2RangesRTK;

    data->time.tv_sec = bestGpsPos->timeStampSec;
    data->time.tv_usec = bestGpsPos->timeStampUSec;


    double dt = (bestGpsPos->msIntoWeek - bestGpsVel->msIntoWeek)/1000.0;
    if(fabs(dt) > 1.0){
        std::stringstream ss;
        ss << "GPS position/velocity time stamp mismatch! dt: " << dt << "ms";
        //context_.tracer().warning(ss.str());
        std::cerr << ss.str();
    }
    if(bestGpsPos->positionType != bestGpsVel->velocityType){
        std::stringstream ss;
        ss << "GPS position/velocity type mismatch! pos_type: " <<bestGpsPos->positionType<<", vel_type: " <<bestGpsVel->velocityType;
        //context_.tracer().warning(ss.str());
        std::cerr << ss.str();
    }
    if(bestGpsPos->solutionStatus != bestGpsVel->solutionStatus){
        std::stringstream ss;
        ss << "GPS position/velocity solution status mismatch! sol_status (pos): " <<bestGpsPos->solutionStatus<<", sol_status (vel): " <<bestGpsVel->solutionStatus;
        //context_.tracer().warning(ss.str());
        std::cerr << ss.str();
    }

    switch(bestGpsPos->positionType){
        case gna::None:
        case gna::Ins:          // yep, this counts as NoFix; the numbers are probably ok, but they come from the INS, _not_ a GPS fix
        case gna::Propagated:   // same
        case gna::RtkDirectIns: // same
        case gna::InsRtkFloat:  // same
        case gna::InsRtkFixed:  // same
            data->gpsSolutionStatus = hif::InsGps::NoFix;
            data->gpsCorrectionMethod = hif::InsGps::NoDiff;
            break;

        case gna::FixedPos:
        case gna::FixedHeight:
            //debatable; In a (mobile) robotics context I'd label it bad.
            data->gpsSolutionStatus = hif::InsGps::BadFix;
            data->gpsCorrectionMethod = hif::InsGps::NoDiff;
            break;

        case gna::Single:
        case gna::DopplerVelocity:
        case gna::InsPsrSp:
            data->gpsSolutionStatus = hif::InsGps::GoodFix;
            data->gpsCorrectionMethod = hif::InsGps::NoDiff;
            break;

        case gna::PsrDiff:
        case gna::InsPsrDiff:
            data->gpsSolutionStatus = hif::InsGps::GoodFix;
            data->gpsCorrectionMethod = hif::InsGps::CodeDiff;
            data->gpsCorrectionSource.resize(1);
            data->gpsCorrectionSource[0] = hif::InsGps::Local;
            break;

        case gna::Waas:
        case gna::Omnistar:
        case gna::CdGps:
            data->gpsSolutionStatus = hif::InsGps::GoodFix;
            data->gpsCorrectionMethod = hif::InsGps::CodeDiff;
            data->gpsCorrectionSource.resize(1);
            data->gpsCorrectionSource[0] = hif::InsGps::Satellite;
            break;

        case gna::FloatConv:
        case gna::WideLane:
        case gna::NarrowLane:
        case gna::L1Float:
        case gna::IonoFreeFloat:
        case gna::NarrowFloat:
        case gna::L1Int:
        case gna::WideInt:
        case gna::NarrowInt:
            data->gpsSolutionStatus = hif::InsGps::GoodFix;
            data->gpsCorrectionMethod = hif::InsGps::CarrierDiff;
            data->gpsCorrectionSource.resize(1);
            data->gpsCorrectionSource[0] = hif::InsGps::Local;
            break;

        case gna::OmniStarHp:
        case gna::OmniStarXp:
            data->gpsSolutionStatus = hif::InsGps::GoodFix;
            data->gpsCorrectionMethod = hif::InsGps::CarrierDiff;
            data->gpsCorrectionSource.resize(1);
            data->gpsCorrectionSource[0] = hif::InsGps::Satellite;
            break;

        default:
            data->gpsSolutionStatus = hif::InsGps::NoFix;
            data->gpsCorrectionMethod = hif::InsGps::NoDiff;
            break;

    }



    switch(bestGpsPos->solutionStatus){
        case gna::SolComputed:
            data->gpsSolutionStatus = hif::InsGps::GoodFix; // really??
            break;

        case gna::ColdStart:
        case gna::VHLimit:
        case gna::InsufficientObs:
            data->gpsSolutionStatus = hif::InsGps::NoFix;
            break;

        case gna::NoConvergence:
        case gna::Singularity:
        case gna::CovTrace:
        case gna::TestDist:
        case gna::Variance:
        case gna::Residuals:
        case gna::DeltaPos:
        case gna::NegativeVar:
        case gna::IntegrityWarning:
        case gna::Pending:
        case gna::InvalidFix:
            //if anything looks fishy, we label it as bad
            data->gpsSolutionStatus = hif::InsGps::BadFix;
            break;

        case gna::InsInactive:
        case gna::InsAligning:
        case gna::InsBad:
        case gna::ImuUnplugged: //these guys have fuck all to do with GPS, but are intermixed here anyhow
        default:
            data->gpsSolutionStatus = hif::InsGps::NoFix; //if in doubt we ignore it; might have to rethink if it occurs often
            {
                std::stringstream ss;
                ss << "sol_status unknown or not GPS related: " << bestGpsPos->solutionStatus << "setting gpsSolutionStatus to NoFix";
                //context_.tracer().warning(ss.str());
                std::cerr << ss.str();
            }
            break;
    }
    if(hif::InsGps::NoDiff != data->gpsCorrectionMethod){
        data->gpsCorrectionAge.resize(1);
        data->gpsCorrectionAge[0] = bestGpsPos->diffAge;
        data->gpsBaseStationId.assign(bestGpsPos->baseStationId,4);
    }
    gbx2Hydro(data->statusMessageType, data->statusMessage, bestGpsPos->statusMessageType, bestGpsPos->statusMessage);
    return genericData;
}

std::auto_ptr<hif::InsGps::GenericData>
gbx2Hydro(gna::RawImuData *rawImu){
    hif::InsGps::ImuData *data = new hif::InsGps::ImuData;
    std::auto_ptr<hif::InsGps::GenericData> genericData( data );

    double dt = 0.01;
    // Divide by dt to get the correct units for the IMU data
    data->turnRate[0] = rawImu->xDeltaAng/dt;
    data->turnRate[1] = rawImu->yDeltaAng/dt;
    data->turnRate[2] = rawImu->zDeltaAng/dt;

    data->acc[0] = rawImu->xDeltaV/dt;
    data->acc[1] = rawImu->yDeltaV/dt;
    data->acc[2] = rawImu->zDeltaV/dt;
    data->biasCorrected = false; //really ?? check
    data->tempr.resize(0);

    data->time.tv_sec = rawImu->timeStampSec;
    data->time.tv_usec = rawImu->timeStampUSec;
    gbx2Hydro(data->statusMessageType, data->statusMessage, rawImu->statusMessageType, rawImu->statusMessage);

    return genericData;
}

void gbx2Hydro( enum hif::InsGps::StatusMessageType &hydroStatusType,
        std::string &hydroStatusMessage,
        enum gna::StatusMessageType &gbxStatusType,
        std::string &gbxStatusMessage){
    hydroStatusMessage = gbxStatusMessage;
    switch(gbxStatusType){
        case gna::NoMsg:
            hydroStatusType = hif::InsGps::NoMsg;
            break;
        case gna::Initialising:
            hydroStatusType = hif::InsGps::Initialising;
            break;
        case gna::Ok:
            hydroStatusType = hif::InsGps::Ok;
            break;
        case gna::Warning:
            hydroStatusType = hif::InsGps::Warning;
            break;
        case gna::Fault:
            hydroStatusType = hif::InsGps::Fault;
            break;
        default:
            {
                std::stringstream ss;
                ss <<  "Ins Status: Unknown StatusMessageType. Check gbxnovatelacfr docs for \""
                    << gbxStatusType
                    << "\" as StatusMessageType. Associated Message is: [ "
                    << gbxStatusMessage
                    << " ]";
                hydroStatusMessage = ss.str();
                hydroStatusType = hif::InsGps::Warning;
            }
            break;
    }
    return;
}



} //namespace

extern "C" {
    hif::InsGpsFactory *createDriverFactory()
    { return new insgpsnovatelgbx::Factory; }
}
