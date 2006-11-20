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
#include <orcaice/orcaice.h>

#include "rmpusbdriverconfig.h"
#include "rmpdefs.h"

using namespace std;
using namespace segwayrmp;

// Returns -1 on error
int gainScheduleAsInt( std::string gainSchedule )
{
    if ( gainSchedule == "normal" )
        return RMP_GAIN_SCHEDULE_NORMAL;
    else if ( gainSchedule == "tall" )
        return RMP_GAIN_SCHEDULE_TALL;
    else if ( gainSchedule == "heavy" )
        return RMP_GAIN_SCHEDULE_HEAVY;
    else
    {
        // bad gain schedule
        return -1;
    }
}

std::string gainScheduleAsString( int gainSchedule )
{
    if ( gainSchedule == RMP_GAIN_SCHEDULE_NORMAL )
        return "normal";
    else if ( gainSchedule == RMP_GAIN_SCHEDULE_TALL )
        return "tall";
    else if ( gainSchedule == RMP_GAIN_SCHEDULE_HEAVY )
        return "heavy";
    else
        return "error: bad gain schedule";
}

RmpUsbDriverConfig::RmpUsbDriverConfig()
{
    gainSchedule            = 0;
    maxVelocityScale        = 0.75;
    maxTurnrateScale        = 0.75;
    maxAccelerationScale    = 0.75;
    maxCurrentLimitScale    = 0.75;
}

int
RmpUsbDriverConfig::checkSanity( std::string &warnings, std::string &errors )
{
    std::stringstream ssWarn, ssErr;

    warnings = ssWarn.str();
    errors   = ssErr.str();

    if ( warnings == "" && errors == "" ) return 0;
    return -1;
}

std::ostream &
segwayrmp::operator<<( std::ostream &s, const RmpUsbDriverConfig &c )
{
    s << "RmpUsbDriver Configuration Parameters:     "                     <<endl
      << "\tgainSchedule:              " << gainScheduleAsString(c.gainSchedule) <<endl
      << "\tmaxVelocityScale:          " << c.maxVelocityScale          <<endl
      << "\tmaxTurnrateScale:          " << c.maxTurnrateScale          <<endl
      << "\tmaxAccelerationScale:      " << c.maxAccelerationScale      <<endl
      << "\tmaxCurrentLimitScale:      " << c.maxCurrentLimitScale;

    return s;
}

void
segwayrmp::readFromProperties( const orcaice::Context & context, RmpUsbDriverConfig & c )
{
    Ice::PropertiesPtr prop = context.properties();
    std::string prefix = context.tag() + ".Config.SegwayRmpUsb.";

    std::string gainSchedule = orcaice::getPropertyWithDefault( prop, prefix+"GainSchedule", "normal" );

    c.gainSchedule = gainScheduleAsInt( gainSchedule );

    if ( c.gainSchedule == -1 )
    {
        string errorStr = "SegwayRmpUsb driver: unknown gain schedule type '"+gainSchedule+"'. Cannot initialize.";
        context.tracer()->error( errorStr);
        context.tracer()->info( "Valid gain schedule types are {'normal', 'tall', 'heavy'}" );
        throw orcaice::Exception( ERROR_INFO, errorStr );
    }

    c.maxVelocityScale = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"MaxVelocityScale", 0.75 );
    c.maxTurnrateScale = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"MaxTurnrateScale", 0.75 );
    c.maxAccelerationScale = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"MaxAccelerationScale", 0.75 );
    c.maxCurrentLimitScale = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"MaxCurrentLimitScale", 0.75 );
}
