/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <hydroqgui/exceptions.h>
#include <orcaobj/bros1.h>
#include <orcaobj/localise3d.h>
#include "localise3delement.h"

using namespace std;

namespace orcaqgui3d {

void
Localise3dElement::actionOnConnection()
{
    if (!isConnected_) return;

    tryToGetGeometry();
}

void 
Localise3dElement::tryToGetGeometry()
{
    orca::VehicleGeometryDescriptionPtr geom;
    
    if ( listener_.proxy()==0 )
    {
        humanManager_->showStatusError( "Localise3dElement::tryToGetGeometry(): listener_.proxy()==0" );
    }
    try 
    {
        geom = listener_.proxy()->getDescription();
        if ( geom == 0 )
        {
            throw gbxutilacfr::Exception( ERROR_INFO, "Localise3dElement::tryToGetGeometry(): got NULL vehicle geometry from remote interface!" );
        }
        haveGeometry_ = true;
    }
    catch ( std::exception &e)
    {
        humanManager_->showStatusError( "Localise3dElement::tryToGetGeometry(): Exception when trying to get geometry: " + QString(e.what()) );
    }
    
    if (haveGeometry_ && geom->type==orca::VehicleGeometryCuboid)
    {
        orca::VehicleGeometryCuboidDescriptionPtr cuboidGeom = orca::VehicleGeometryCuboidDescriptionPtr::dynamicCast( geom );
        painter_.setCubicDescr( cuboidGeom->size.l,
                                cuboidGeom->size.w,
                                cuboidGeom->size.h,
                                cuboidGeom->platformToGeometryTransform );
    }
    else if (haveGeometry_ && geom->type==orca::VehicleGeometryCylindrical)
    {
        orca::VehicleGeometryCylindricalDescriptionPtr cylGeom = orca::VehicleGeometryCylindricalDescriptionPtr::dynamicCast( geom );
        painter_.setCylindricalDescr( cylGeom->radius,
                                      cylGeom->height,
                                      cylGeom->platformToGeometryTransform );
    }
    else
    {
        humanManager_->showStatusWarning("Localise3dElement: unknown geometry!");
        const double radius = 0.375;
        const double height = 1.0;
        painter_.setCylindricalDescr( radius, height, orcaobj::zeroFrame3d() );
    }
}

void
Localise3dElement::update()
{
    // standard update as in IceStormElement3d
    if ( !orcaqgui3d::IceStormElement3d<LocalisePainter,
            orca::Localise3dData,
            orca::Localise3dPrx,
            orca::Localise3dConsumer,
            orca::Localise3dConsumerPrx>::needToUpdate() )
        return;

    assert( !listener_.store().isEmpty() );
    
    listener_.store().get( data_ );
    painter_.setData( data_ );

    // custom update, but first error-check.
    if ( data_.hypotheses.size() == 0 )
    {
        std::stringstream ss;
        ss << "Localise3dElement::update(): Interface " << listener_.interfaceName() << ": Localise3dData had zero hypotheses";
        throw hydroqgui::Exception( ERROR_INFO, ss.str() );
    }
    const orca::Pose3dHypothesis &h = orcaobj::mlHypothesis( data_ );
    
    x_     = h.mean.p.x;
    y_     = h.mean.p.y;
    z_     = h.mean.p.z;
    roll_  = h.mean.o.r;
    pitch_ = h.mean.o.p;
    yaw_   = h.mean.o.y;
}

QStringList
Localise3dElement::contextMenu()
{
    QStringList s;
    s<<"Toggle Multi-Hypothesis";
    return s;
}

void
Localise3dElement::execute( int action )
{
    switch ( action )
    {
        case 0 :
        {
            painter_.toggleMultiHypothesis();
            break;
        }
        default:
        {
            throw hydroqgui::Exception( ERROR_INFO, "execute(): What the hell? bad action." );
            break;
        }
    }
}

}
