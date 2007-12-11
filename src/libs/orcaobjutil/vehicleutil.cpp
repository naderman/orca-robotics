/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include "vehicleutil.h"
#include <iostream>
#include <orcaice/orcaice.h>

using namespace std;

namespace orcaobjutil {

void
readVehicleControlVelocityDifferentialDescription( Ice::PropertiesPtr prop,
                                                   const std::string &prefix,
                                                   orca::VehicleControlVelocityDifferentialDescription &c )
{
    std::string cprefix = prefix + "Control.VelocityDifferential.";
    c.type = orca::VehicleControlVelocityDifferential;
    c.maxForwardSpeed = orcaice::getPropertyAsDoubleWithDefault( prop, cprefix+"MaxForwardSpeed", 1.0 );
    c.maxReverseSpeed = orcaice::getPropertyAsDoubleWithDefault( prop, cprefix+"MaxReverseSpeed", 1.0 );
    c.maxTurnrate     = DEG2RAD(orcaice::getPropertyAsDoubleWithDefault( prop, cprefix+"MaxTurnrate", 60.0 ));
    c.maxLateralAcceleration  = orcaice::getPropertyAsDoubleWithDefault( prop, cprefix+"MaxLateralAcceleration", 3.57 );
    c.maxForwardAcceleration = orcaice::getPropertyAsDoubleWithDefault( prop, cprefix+"MaxForwardAcceleration", 1.0 );
    c.maxReverseAcceleration = orcaice::getPropertyAsDoubleWithDefault( prop, cprefix+"MaxReverseAcceleration", 1.0 );
    c.maxRotationalAcceleration = DEG2RAD(orcaice::getPropertyAsDoubleWithDefault( prop, cprefix+"MaxRotationalAcceleration", 60.0 ));
    checkVehicleControlVelocityDifferentialDescription( c );
}

void
readVehicleControlVelocityBicycleDescription( Ice::PropertiesPtr prop,
                                                   const std::string &prefix,
                                                   orca::VehicleControlVelocityBicycleDescription &c )
{
    std::string cprefix = prefix + "Control.VelocityBicycle.";
    c.type = orca::VehicleControlVelocityBicycle;
    c.maxForwardSpeed = orcaice::getPropertyAsDoubleWithDefault( prop, cprefix+"MaxForwardSpeed", 1.0 );
    c.maxReverseSpeed = orcaice::getPropertyAsDoubleWithDefault( prop, cprefix+"MaxReverseSpeed", 1.0 );
    c.maxSteerAngle   = DEG2RAD(orcaice::getPropertyAsDoubleWithDefault( prop, cprefix+"MaxSteerAngle", 45.0 ));
    c.maxSteerAngleAtMaxSpeed  = DEG2RAD(orcaice::getPropertyAsDoubleWithDefault( prop, cprefix+"MaxSteerAngleAtMaxSpeed", 10.0 ));
    c.maxForwardAcceleration = orcaice::getPropertyAsDoubleWithDefault( prop, cprefix+"MaxForwardAcceleration", 1.0 );
    c.maxReverseAcceleration = orcaice::getPropertyAsDoubleWithDefault( prop, cprefix+"MaxReverseAcceleration", 1.0 );
    c.maxSteerAngleRate = DEG2RAD(orcaice::getPropertyAsDoubleWithDefault( prop, cprefix+"MaxSteerAngleRate", 20.0 ));
    checkVehicleControlVelocityBicycleDescription( c );

}

void
readVehicleDescription( Ice::PropertiesPtr prop, const std::string &prefix, orca::VehicleDescription &descr )
{
    std::string vprefix = prefix + "VehicleDescription.";

    std::string controlType = orcaice::getPropertyWithDefault( prop, vprefix+"Control.Type", "VelocityDifferential" );
    if ( controlType == "VelocityDifferential" )
    {
        orca::VehicleControlVelocityDifferentialDescription *c = new orca::VehicleControlVelocityDifferentialDescription;
        readVehicleControlVelocityDifferentialDescription( prop, vprefix, *c );
        descr.control = c;
    }
    else if ( controlType == "VelocityBicycle" )
    {
        orca::VehicleControlVelocityBicycleDescription *c = new orca::VehicleControlVelocityBicycleDescription;
        readVehicleControlVelocityBicycleDescription( prop, vprefix, *c );
        descr.control = c;
    }
    else
    {
        stringstream ss; ss << "Unsupported Control Type: " << controlType;
        throw orcaice::ConfigFileException( ERROR_INFO, ss.str() );
    }

    orcaice::setInit( descr.platformToVehicleTransform );
    descr.platformToVehicleTransform = orcaice::getPropertyAsFrame3dWithDefault( 
                prop, vprefix+"PlatformToVehicleTransform", descr.platformToVehicleTransform );

    std::string geometryType = orcaice::getPropertyWithDefault( prop, vprefix+"Geometry.Type", "Cylindrical" );
    if ( geometryType == "Cylindrical" )
    {
        std::string gprefix = vprefix+"Geometry.Cylindrical.";

        orca::VehicleGeometryCylindricalDescriptionPtr g = new orca::VehicleGeometryCylindricalDescription;
        g->type = orca::VehicleGeometryCylindrical;

        g->radius = orcaice::getPropertyAsDoubleWithDefault( prop, gprefix+"Radius", 0.25 );
        g->height = orcaice::getPropertyAsDoubleWithDefault( prop, gprefix+"Height", 1.0 );

        orcaice::setInit( g->vehicleToGeometryTransform );
        g->vehicleToGeometryTransform = orcaice::getPropertyAsFrame3dWithDefault( 
                    prop, gprefix+"VehicleToGeometryTransform", g->vehicleToGeometryTransform );

        descr.geometry = g;
    }
    else if ( geometryType == "Cuboid" )
    {
        std::string gprefix = vprefix+"Geometry.Cuboid.";

        orca::VehicleGeometryCuboidDescriptionPtr g = new orca::VehicleGeometryCuboidDescription;
        g->type = orca::VehicleGeometryCuboid;

        orcaice::setInit( g->size );
        g->size = orcaice::getPropertyAsSize3dWithDefault( prop, gprefix+"Size", g->size );

        orcaice::setInit( g->vehicleToGeometryTransform );
        g->vehicleToGeometryTransform = orcaice::getPropertyAsFrame3dWithDefault( 
                    prop, gprefix+"VehicleToGeometryTransform", g->vehicleToGeometryTransform );

        descr.geometry = g;
    }
    else
    {
        stringstream ss; ss << "Unsupported Geometry Type: " << controlType;
        throw orcaice::ConfigFileException( ERROR_INFO, ss.str() );
    }

    assert( descr.control != 0 );
    assert( descr.geometry != 0 );
}


// Throws orcaice::ConfigFileException if something is wrong.
void 
checkVehicleControlVelocityDifferentialDescription( 
    const orca::VehicleControlVelocityDifferentialDescription &d )
{
    if ( d.maxForwardSpeed < 0.0 ||
         d.maxReverseSpeed < 0.0 ||
         d.maxTurnrate < 0.0 ||
         d.maxLateralAcceleration < 0.0 ||
         d.maxForwardAcceleration < 0.0 ||
         d.maxReverseAcceleration < 0.0 ||
         d.maxRotationalAcceleration < 0.0 )
    {
        throw orcaice::ConfigFileException( ERROR_INFO, "Negative speed/velocity/acceleration found in orca::VehicleControlVelocityDifferentialDescription" );
    }
}

// Throws orcaice::ConfigFileException if something is wrong.
void 
checkVehicleControlVelocityBicycleDescription( 
    const orca::VehicleControlVelocityBicycleDescription &d )
{
    if ( d.maxForwardSpeed < 0.0 ||
         d.maxReverseSpeed < 0.0 ||
         d.maxSteerAngle < 0.0 ||
         d.maxSteerAngleAtMaxSpeed < 0.0 ||
         d.maxForwardAcceleration < 0.0 ||
         d.maxReverseAcceleration < 0.0 ||
         d.maxSteerAngleRate < 0.0 )
    {
        throw orcaice::ConfigFileException( ERROR_INFO, "Negative speed or velocity found in orca::VehicleControlVelocityBicycleDescription" );
    }

    if ( d.maxSteerAngle < d.maxSteerAngleAtMaxSpeed )
    {
        throw orcaice::ConfigFileException( ERROR_INFO, "orca::VehicleControlVelocityBicycleDescription specifies max steer angle at max speed greater than at 0m/s" );
    }
}

}
