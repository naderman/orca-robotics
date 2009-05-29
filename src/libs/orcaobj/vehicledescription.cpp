/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <sstream>
#include <gbxutilacfr/mathdefs.h>
#include <orcaifaceutil/vehicledescription.h>
#include "vehicledescription.h"
#include "bros1.h"

using namespace std;

namespace orcaobj
{

std::string toString( const orca::VehicleDescription& obj )
{
    std::ostringstream s;
    s << "VehicleDescription: " << endl
      << "  " << toString(obj.control) << endl
      << toString(obj.geometry);
    return s.str();
}

std::string toString( const orca::VehicleControlDescriptionPtr& obj )
{
    if ( obj == 0 )
        return "Null VehicleControlDescriptionPtr";
    //throw gbxutilacfr::Exception( ERROR_INFO, "VehicleControlDescriptionPtr was zero!" );

    std::ostringstream s;
    s << "VehicleControlDescription: "
      << "type=" << ifaceutil::toString( obj->type );

    if ( obj->ice_isA( "::orca::VehicleControlVelocityDifferentialDescription" ) )
    {
        orca::VehicleControlVelocityDifferentialDescriptionPtr v = orca::VehicleControlVelocityDifferentialDescriptionPtr::dynamicCast( obj );
        if ( v )
        {
            s << endl << "VehicleControlVelocityDifferentialDescription: " << endl
            << "     maxForwardSpeed:           " << v->maxForwardSpeed                    << "m/s"   << endl
            << "     maxReverseSpeed:           " << v->maxReverseSpeed                    << "m/s"   << endl
            << "     maxTurnrate:               " << RAD2DEG(v->maxTurnrate)               << "deg/s" << endl
            << "     maxLateralAcceleration:    " << v->maxLateralAcceleration             << "m/s/s" << endl
            << "     maxForwardAcceleration:    " << v->maxForwardAcceleration             << "m/s/s" << endl
            << "     maxReverseAcceleration:    " << v->maxReverseAcceleration             << "m/s/s" << endl
            << "     maxRotationalAcceleration: " << RAD2DEG(v->maxRotationalAcceleration) << "deg/s/s";
        }
    }
    else if ( obj->ice_isA( "::orca::VehicleControlVelocityBicycleDescription" ) )
    {
        orca::VehicleControlVelocityBicycleDescriptionPtr v = orca::VehicleControlVelocityBicycleDescriptionPtr::dynamicCast( obj );
        if ( v )
        {        
            s << endl << "VehicleControlVelocityBicycleDescription: " << endl
            << "     maxForwardSpeed:           " << v->maxForwardSpeed                    << "m/s"   << endl
            << "     maxReverseSpeed:           " << v->maxReverseSpeed                    << "m/s"   << endl
            << "     maxSteerAngle:             " << RAD2DEG(v->maxSteerAngle)             << "deg" << endl
            << "     maxSteerAngleAtMaxSpeed:   " << RAD2DEG(v->maxSteerAngleAtMaxSpeed)   << "deg" << endl
            << "     maxForwardAcceleration:    " << v->maxForwardAcceleration             << "m/s"   << endl
            << "     maxReverseAcceleration:    " << v->maxReverseAcceleration             << "m/s"   << endl
            << "     maxSteerAngleRate:         " << RAD2DEG(v->maxSteerAngleRate)         << "deg/s";
        }
    }
    return s.str();
}

std::string toString( const orca::VehicleGeometryDescriptionPtr& obj )
{
    if ( obj == 0 )
        return "Null VehicleGeometryDescriptionPtr";
//        throw gbxutilacfr::Exception( ERROR_INFO, "VehicleGeometryDescriptionPtr was zero!" );

    std::ostringstream s;
    s << "VehicleGeometryDescription: "
      << "type=" << ifaceutil::toString( obj->type );

    if ( obj->ice_isA( "::orca::VehicleGeometryCylindricalDescription" ) )
    {
        orca::VehicleGeometryCylindricalDescriptionPtr v = orca::VehicleGeometryCylindricalDescriptionPtr::dynamicCast( obj );
        if ( v )
        { 
            s << endl << "VehicleGeometryCylindricalDescription: " << endl
            << "  radius: " << v->radius << "m" << endl
            << "  height: " << v->height << "m" << endl
            << "  platformToGeometryTransform: " << toString(v->platformToGeometryTransform);
        }
    }
    else if ( obj->ice_isA( "::orca::VehicleGeometryCuboidDescription" ) )
    {
        orca::VehicleGeometryCuboidDescriptionPtr v = orca::VehicleGeometryCuboidDescriptionPtr::dynamicCast( obj );
        if ( v )
        { 
            s << endl << "VehicleGeometryCuboidDescription: " << endl
            << "  length: " << v->size.l << "m" << endl
            << "  width:  " << v->size.w << "m" << endl
            << "  height: " << v->size.h << "m" << endl
            << "  platformToGeometryTransform: " << toString(v->platformToGeometryTransform);
        }
    }
    return s.str();
}

} // namespace
