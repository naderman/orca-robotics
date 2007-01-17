/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#include "configutils.h"
#include <iostream>
#include <orcaobj/mathdefs.h>
#include <orcaice/orcaice.h>

using namespace std;

namespace orcamisc {

// Throws orcaice::ConfigFileException if something is wrong.
void 
checkVehicleControlVelocityDifferentialDescription( 
    const orca::VehicleControlVelocityDifferentialDescription &d )
{
    if ( d.maxForwardSpeed < 0.0 ||
         d.maxReverseSpeed < 0.0 ||
         d.maxTurnrate < 0.0 ||
         d.maxTurnrate1ms < 0.0 ||
         d.maxForwardAcceleration < 0.0 ||
         d.maxReverseAcceleration < 0.0 ||
         d.maxRotationalAcceleration < 0.0 )
    {
        throw orcaice::ConfigFileException( ERROR_INFO, "Negative speed or velocity found in orca::VehicleControlVelocityDifferentialDescription" );
    }

    if ( d.maxTurnrate < d.maxTurnrate1ms )
    {
        throw orcaice::ConfigFileException( ERROR_INFO, "orca::VehicleControlVelocityDifferentialDescription specifies max turnrate at 1m/s greater that max turnrate at 0m/s" );
    }

    double slope = d.maxTurnrate - d.maxTurnrate1ms;
    if ( d.maxTurnrate - slope * d.maxForwardSpeed < 0.0 ||
         d.maxTurnrate - slope * d.maxReverseSpeed < 0.0 )
    {
        throw orcaice::ConfigFileException( ERROR_INFO, "orca::VehicleControlVelocityDifferentialDescription allows negative maxTurnrate for some speeds." );
    }
}

void
readVehicleDescription( Ice::PropertiesPtr prop, const std::string &prefix, orca::VehicleDescription &descr )
{
    std::string vprefix = prefix + "VehicleDescription.";

    std::string controlType = orcaice::getPropertyWithDefault( prop, vprefix+"Control.Type", "VelocityDifferential" );
    if ( controlType == "VelocityDifferential" )
    {
        std::string cprefix = vprefix + "Control.VelocityDifferential.";

        orca::VehicleControlVelocityDifferentialDescription *c = new orca::VehicleControlVelocityDifferentialDescription;
        c->type = orca::VehicleControlVelocityDifferential;

        c->maxForwardSpeed = orcaice::getPropertyAsDoubleWithDefault( prop, cprefix+"MaxForwardSpeed", 1.0 );
        c->maxReverseSpeed = orcaice::getPropertyAsDoubleWithDefault( prop, cprefix+"MaxReverseSpeed", 1.0 );
        c->maxTurnrate     = DEG2RAD(orcaice::getPropertyAsDoubleWithDefault( prop, cprefix+"MaxTurnrate", 45.0 ));
        c->maxTurnrate1ms  = DEG2RAD(orcaice::getPropertyAsDoubleWithDefault( prop, cprefix+"MaxTurnrate1ms", 45.0 ));
        c->maxForwardAcceleration = orcaice::getPropertyAsDoubleWithDefault( prop, cprefix+"MaxForwardAcceleration", 1.0 );
        c->maxReverseAcceleration = orcaice::getPropertyAsDoubleWithDefault( prop, cprefix+"MaxReverseAcceleration", 1.0 );
        c->maxRotationalAcceleration = DEG2RAD(orcaice::getPropertyAsDoubleWithDefault( prop, cprefix+"MaxRotationalAcceleration", 45.0 ));
        checkVehicleControlVelocityDifferentialDescription( *c );

        descr.control = c;
    }
    else
    {
        stringstream ss; ss << "Unsupported Control Type: " << controlType;
        throw orcaice::ConfigFileException( ERROR_INFO, ss.str() );
    }

    string key = vprefix+"PlatformToVehicleTransform";
    int ret = orcaice::getPropertyAsFrame3d( prop, key, descr.platformToVehicleTransform );
    if ( ret != 0 )
    {
        stringstream ss; ss << "Failed to read config param: " << key;
        throw orcaice::ConfigFileException( ERROR_INFO, ss.str() );
    }

    std::string geometryType = orcaice::getPropertyWithDefault( prop, vprefix+"Geometry.Type", "Cylindrical" );
    if ( geometryType == "Cylindrical" )
    {
        std::string gprefix = vprefix+"Geometry.Cylindrical.";

        orca::VehicleGeometryCylindricalDescription *g = new orca::VehicleGeometryCylindricalDescription;
        g->type = orca::VehicleGeometryCylindrical;

        g->radius = orcaice::getPropertyAsDoubleWithDefault( prop, gprefix+"Radius", 0.25 );
        g->height = orcaice::getPropertyAsDoubleWithDefault( prop, gprefix+"Height", 1.0 );
        ret = orcaice::getPropertyAsFrame3d( prop, key, g->vehicleToGeometryTransform );
        key = gprefix+"VehicleToGeometryTransform";
        if ( ret != 0 )
        {
            stringstream ss; ss << "Failed to read config param: " << key;
            throw orcaice::ConfigFileException( ERROR_INFO, ss.str() );
        }
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

}
