/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */


#include <orcaqgui/exceptions.h>
#include "localise2delement.h"

using namespace std;
using namespace orca;
using namespace orcaqgui2d;
using namespace orcaqgui;

namespace orcaqgui2d {

void
Localise2dElement::actionOnConnection()
{
    if (!isConnected_) return;

    tryToGetGeometry();
    
    paintInitialData<orca::Localise2dPrx, Localise2dPainter>
        ( context_, listener_.interfaceName(), painter_ );
}

void 
Localise2dElement::tryToGetGeometry()
{
    
    VehicleGeometryDescriptionPtr geom;
    
    try 
    {
        geom = listener_.proxy()->getVehicleGeometry();
        haveGeometry_=true;
    }
    catch ( std::exception &e)
    {
        humanManager_->showStatusMsg(Error,"Exception when trying to get geometry: " + QString(e.what()) );
    }
    
    if (!haveGeometry_) {
        painter_.setTypeAndGeometry(PlatformTypeCubic, 0.5, 0.4 );
        painter_.setOrigin( 0.0, 0.0, 0.0 );
        return;
    }
    
    if (geom->type==VehicleGeometryCuboid)
    {
        VehicleGeometryCuboidDescriptionPtr geom = VehicleGeometryCuboidDescriptionPtr::dynamicCast( geom );
        painter_.setTypeAndGeometry( PlatformTypeCubic, geom->size.l, geom->size.w );
        painter_.setOrigin( geom->vehicleToGeometryTransform.p.x, geom->vehicleToGeometryTransform.p.y, geom->vehicleToGeometryTransform.o.y );
    }
    else if (geom->type==VehicleGeometryCylindrical)
    {
        VehicleGeometryCylindricalDescriptionPtr cylGeom = VehicleGeometryCylindricalDescriptionPtr::dynamicCast( geom );
        painter_.setTypeAndGeometry( PlatformTypeCylindrical, cylGeom->radius );
        painter_.setOrigin( cylGeom->vehicleToGeometryTransform.p.x, cylGeom->vehicleToGeometryTransform.p.y, cylGeom->vehicleToGeometryTransform.o.y );
    }
    else
    {
        humanManager_->showStatusMsg(Warning, "Unknown platform type. Will paint a rectangle");
        VehicleGeometryCuboidDescriptionPtr cubGeom = VehicleGeometryCuboidDescriptionPtr::dynamicCast( geom );
        painter_.setTypeAndGeometry( PlatformTypeCubic, cubGeom->size.l, cubGeom->size.w );
        painter_.setOrigin( cubGeom->vehicleToGeometryTransform.p.x, cubGeom->vehicleToGeometryTransform.p.y, cubGeom->vehicleToGeometryTransform.o.y );
    }
}

void
Localise2dElement::update()
{
    if (!haveGeometry_)
        tryToGetGeometry();
    
    // standard update as in IceStormElement
    if ( !IceStormElement<Localise2dPainter,
            orca::Localise2dData,
            orca::Localise2dPrx,
            orca::Localise2dConsumer,
            orca::Localise2dConsumerPrx>::needToUpdate() )
        return;

    assert( !listener_.buffer().isEmpty() );
    
    listener_.buffer().getAndPop( data_ );
    painter_.setData( data_ );

    // custom update, but first error-check.
    if ( data_.hypotheses.size() == 0 )
    {
        std::stringstream ss;
        ss << "Localise2dElement::update(): Interface " << listener_.interfaceName() << ": Localise2dData had zero hypotheses";
        throw orcaqgui::Exception( ss.str() );
    }
    const orca::Pose2dHypothesis &h = orcaice::mlHypothesis( data_ );
    
    x_ = h.mean.p.x;
    y_ = h.mean.p.y;
    theta_ = h.mean.o;
}

QStringList
Localise2dElement::contextMenu()
{
    QStringList s;
    s<<"Toggle History"<<"Toggle Multi-Hypothesis";
    return s;
}

void
Localise2dElement::execute( int action )
{
    switch ( action )
    {
        case 0 :
        {
            painter_.toggleDisplayHistory();
            break;
        }
        case 1 :
        {
            painter_.toggleMultiHypothesis();
            break;
        }
        default:
        {
            throw orcaqgui::Exception( "execute(): What the hell? bad action." );
            break;
        }
    }
}

}
