/*
 *  Orca Project: Components for robotics.
 *
 *  Copyright (C) 2004-2006
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <iostream>
#include <orcaice/orcaice.h>

#include "usbdriverconfig.h"
#include "rmpdefs.h"

using namespace std;
using namespace robot2d;

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

UsbDriverConfig::UsbDriverConfig()
{
    gainSchedule            = 0;
    maxVelocityScale        = 0.75;
    maxTurnrateScale        = 0.75;
    maxAccelerationScale    = 0.75;
    maxCurrentLimitScale    = 0.75;
}

int
UsbDriverConfig::checkSanity( std::string &warnings, std::string &errors )
{
    std::stringstream ssWarn, ssErr;

    warnings = ssWarn.str();
    errors   = ssErr.str();

    if ( warnings == "" && errors == "" ) return 0;
    return -1;
}

std::ostream &
robot2d::operator<<( std::ostream &s, const UsbDriverConfig &c )
{
    s << "UsbDriver Configuration Parameters:     "                     <<endl
      << "\tgainSchedule:              " << gainScheduleAsString(c.gainSchedule) <<endl
      << "\tmaxVelocityScale:          " << c.maxVelocityScale          <<endl
      << "\tmaxTurnrateScale:          " << c.maxTurnrateScale          <<endl
      << "\tmaxAccelerationScale:      " << c.maxAccelerationScale      <<endl
      << "\tmaxCurrentLimitScale:      " << c.maxCurrentLimitScale;

    return s;
}

void
robot2d::readFromProperties( const orcaice::Context & context, UsbDriverConfig & c )
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
