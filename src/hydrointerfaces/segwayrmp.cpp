/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <sstream>
#include "segwayrmp.h"
#include <assert.h>
#include <gbxutilacfr/mathdefs.h>

static const double Pi = 3.14159265358979323846264338327950288419717;

namespace hydrointerfaces {

std::string 
SegwayRmp::Data::toString() const
{
    std::stringstream ss;

    ss << "  seconds:     " << seconds  << std::endl;
    ss << "  useconds:    " << useconds << std::endl;
    ss << "  x:           " << x        << std::endl;
    ss << "  y:           " << y        << std::endl;
    ss << "  roll:        " << roll     << std::endl;
    ss << "  pitch:       " << pitch    << std::endl;
    ss << "  yaw:         " << yaw      << std::endl;
    ss << "  vx:          " << vx       << std::endl;
    ss << "  droll:       " << droll    << std::endl;
    ss << "  dpitch:      " << dpitch   << std::endl;
    ss << "  dyaw:        " << dyaw     << std::endl;
    ss << "  leftTorque:  " << leftTorque << std::endl;
    ss << "  rightTorque: " << rightTorque << std::endl;
    ss << "  mainvolt:    " << mainvolt << std::endl;
    ss << "  uivolt:      " << uivolt   << std::endl;
    if ( hasWarnings ) ss << "WARN";
    if ( hasWarnings && hasFaults ) ss << "/";
    if ( hasFaults ) ss << "FAULT";
    if ( hasWarnings || hasFaults )
        ss << ": " << warnFaultReason << std::endl;
    return ss.str(); 
}

std::string 
SegwayRmp::Command::toString() const
{
    std::stringstream ss;
    ss << "[ "<<vx<<"m/s, "<<w*180.0/Pi<<"deg/s ]";

    return ss.str();
}

bool
SegwayRmp::Capabilities::isSane() const
{
    return ( maxForwardSpeed >= 0.0 &&
             maxReverseSpeed >= 0.0 &&
             maxTurnrate >= 0.0 &&
             maxLateralAcceleration >= 0.0 );
}

std::string
SegwayRmp::Capabilities::toString() const
{
    std::stringstream ss;
    ss << std::endl
       << "  maxForwardSpeed: " << maxForwardSpeed << "m/s" << std::endl
       << "  maxReverseSpeed: " << maxReverseSpeed << "m/s" << std::endl
       << "  maxTurnrate:     " << maxTurnrate*180.0/M_PI << "deg/s" << std::endl
       << "  maxLateralAcceleration: " << maxLateralAcceleration << "m/s/s";
    return ss.str();
}

void
constrain( SegwayRmp::Capabilities &capabilities, 
           const SegwayRmp::Capabilities &limits )
{
    assert( capabilities.isSane() && limits.isSane() );

    capabilities.maxForwardSpeed = MIN( capabilities.maxForwardSpeed, limits.maxForwardSpeed );
    capabilities.maxReverseSpeed = MIN( capabilities.maxReverseSpeed, limits.maxReverseSpeed );
    capabilities.maxTurnrate = MIN( capabilities.maxTurnrate, limits.maxTurnrate );
    capabilities.maxLateralAcceleration = MIN( capabilities.maxLateralAcceleration, limits.maxLateralAcceleration );
}

bool operator==( const SegwayRmp::Capabilities &c1, const SegwayRmp::Capabilities &c2 )
{
    const double EPS = 1e-5;
    return ( NEAR( c1.maxForwardSpeed,        c2.maxForwardSpeed, EPS ) &&
             NEAR( c1.maxReverseSpeed,        c2.maxReverseSpeed, EPS ) &&
             NEAR( c1.maxTurnrate,            c2.maxTurnrate, EPS ) &&
             NEAR( c1.maxLateralAcceleration, c2.maxLateralAcceleration, EPS ) );
}

bool commandPossible( const hydrointerfaces::SegwayRmp::Command      &cmd,
                      const hydrointerfaces::SegwayRmp::Capabilities &capabilities )
{
    if ( cmd.vx >  capabilities.maxForwardSpeed ) return false;
    if ( cmd.vx < -capabilities.maxReverseSpeed ) return false;
    if ( cmd.w >  capabilities.maxTurnrate ) return false;
    if ( cmd.w < -capabilities.maxTurnrate ) return false;        

    const double lateralAcceleration = cmd.vx*cmd.w;
    if ( lateralAcceleration > capabilities.maxLateralAcceleration ) return false;

    return true;
}

void limit( hydrointerfaces::SegwayRmp::Command            &cmd,
            const hydrointerfaces::SegwayRmp::Capabilities &capabilities )
{
    // Note that maxReverseSpeed is a positive number.
    CLIP_TO_LIMITS( -capabilities.maxReverseSpeed, cmd.vx, capabilities.maxForwardSpeed );
    CLIP_TO_LIMITS( -capabilities.maxTurnrate, cmd.w, capabilities.maxTurnrate );
    
    const double maxTurnrateToSatisfyLateralAcc = capabilities.maxLateralAcceleration / fabs(cmd.vx);
    CLIP_TO_LIMITS( -maxTurnrateToSatisfyLateralAcc, cmd.w, maxTurnrateToSatisfyLateralAcc );
}


} // namespace
