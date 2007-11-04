/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>

#include "rmpdriverconfig.h"
#include "rmpdefs.h"

using namespace std;
using namespace segwayrmp;

namespace {

GainSchedule gainScheduleAsEnum( const std::string &gainSchedule )
{
    if ( gainSchedule == "normal" )
        return GainScheduleNormal;
    else if ( gainSchedule == "tall" )
        return GainScheduleTall;
    else if ( gainSchedule == "heavy" )
        return GainScheduleHeavy;
    else
    {
        stringstream ss;
        ss << "Unknown GainSchedule: " << gainSchedule << endl
           << "Valid gain schedule types are {'normal', 'tall', 'heavy'}";
        throw hydroutil::Exception( ERROR_INFO, ss.str() );
    }
}

std::string gainScheduleAsString( GainSchedule gainSchedule )
{
    if ( gainSchedule == GainScheduleNormal )
        return "normal";
    else if ( gainSchedule == GainScheduleTall )
        return "tall";
    else if ( gainSchedule == GainScheduleHeavy )
        return "heavy";
    else
    {
        stringstream ss;
        ss << "Unknown GainSchedule: " << gainSchedule;
        throw hydroutil::Exception( ERROR_INFO, ss.str() );
    }
}

RmpModel modelAsEnum( const std::string &m )
{
    if ( m=="RMP50" )
        return RmpModel_50;
    else if ( m == "RMP100" )
        return RmpModel_100;
    else if ( m == "RMP200" )
        return RmpModel_200;
    else if ( m == "RMP400" )
        return RmpModel_400;
    else
    {
        stringstream ss;
        ss << "Unknown RMP model number: " << model << endl
           << "Valid values are {'RMP50','RMP100','RMP200','RMP400'}";
        throw hydroutil::Exception( ERROR_INFO, ss.str() );
    }
}
}

}

RmpDriverConfig::RmpDriverConfig()
{
    // Invalid values to make sure the config is read properly.
    maxVelocityScale        = -1;
    maxTurnrateScale        = -1;
    maxAccelerationScale    = -1;
    maxCurrentLimitScale    = -1;
}

int
RmpDriverConfig::checkSanity( std::string &errors )
{
    std::stringstream ssErr;

    if ( maxVelocityScale < 0.0 || maxVelocityScale > 1.0 )
        ssErr << "Bad maxVelocityScale.  ";

    if ( maxTurnrateScale < 0.0 || maxTurnrateScale > 1.0 )
        ssErr << "Bad maxTurnrateScale.  ";

    if ( maxAccelerationScale < 0.0 || maxAccelerationScale > 1.0 )
        ssErr << "Bad maxAccelerationScale.  ";

    if ( maxCurrentLimitScale < 0.0 || maxCurrentLimitScale > 1.0 )
        ssErr << "Bad maxCurrentLimitScale.  ";

    errors   = ssErr.str();

    if ( errors == "" ) return 0;
    else return -1;
}

std::ostream &
segwayrmp::operator<<( std::ostream &s, const RmpDriverConfig &c )
{
    s << "RmpDriver Configuration Parameters:     "                     <<endl
      << "\t gainSchedule:              " << gainScheduleAsString(c.gainSchedule) <<endl
      << "\t maxVelocityScale:          " << c.maxVelocityScale          <<endl
      << "\t maxTurnrateScale:          " << c.maxTurnrateScale          <<endl
      << "\t maxAccelerationScale:      " << c.maxAccelerationScale      <<endl
      << "\t maxCurrentLimitScale:      " << c.maxCurrentLimitScale      <<endl
      << "\t model:                     " << toString(c.model)           <<endl
      << "\t requireSpecificBuildId:    " << c.requireSpecificBuildId    <<endl;
    if ( c.requireSpecificBuildId )
    {
        s << "\t requiredBuildId:           " << c.requiredBuildId           <<endl;
    }
    s << "\t allowMoveInTractorMode:    " << c.allowMoveInTractorMode    <<endl
      << "\t allowMoveInBalanceMode:    " << c.allowMoveInBalanceMode;

    return s;
}

void
segwayrmp::readFromProperties( const orcaice::Context & context, RmpDriverConfig & c )
{
    Ice::PropertiesPtr prop = context.properties();
    std::string prefix = context.tag() + ".Config.SegwayRmp.";

    std::string gainSchedule = orcaice::getPropertyWithDefault( prop, prefix+"GainSchedule", "normal" );
    c.gainSchedule = gainScheduleAsEnum( gainSchedule );

    c.maxVelocityScale = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"MaxVelocityScale", 0.75 );
    c.maxTurnrateScale = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"MaxTurnrateScale", 0.75 );
    c.maxAccelerationScale = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"MaxAccelerationScale", 0.75 );
    c.maxCurrentLimitScale = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"MaxCurrentLimitScale", 0.75 );

    int modelNum = orcaice::getPropertyAsIntWithDefault( prop, prefix+"ModelNum", 200 );
    c.model = modelAsEnum( modelNum );

    c.requireSpecificBuildId = orcaice::getPropertyAsIntWithDefault( prop, prefix+"RequireSpecificBuildId", 0 );
    c.requiredBuildId = orcaice::getPropertyAsIntWithDefault( prop, prefix+"RequiredBuildId", 0 );

    c.allowMoveInTractorMode = orcaice::getPropertyAsIntWithDefault( prop, prefix+"AllowMoveInTractorMode", 1 );
    c.allowMoveInBalanceMode = orcaice::getPropertyAsIntWithDefault( prop, prefix+"AllowMoveInBalanceMode", 1 );
}
