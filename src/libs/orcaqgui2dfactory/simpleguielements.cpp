/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <orcaobj/miscutils.h>
#include "simpleguielements.h"

using namespace std;
using namespace orca;
using namespace orcaqgui2d;

namespace orcaqgui2d {

void
LaserScanner2dElement::getLaserInfo()
{
    // Subscribe directly to get geometry etc
    LaserScanner2dPrx laserPrx;

    // Don't bother catching exceptions: they'll get caught higher up.

    orcaice::connectToInterfaceWithString( context_, laserPrx, listener_.interfaceName() );
    
    RangeScanner2dDescription descr;
    descr = laserPrx->getDescription();

    // we may get an exception if the laser is not mounted horizontal
    // we display a warning but paint it anyway.
    try
    {
        painter_.setOffset( descr.offset );
    }
    catch ( const orcaqgui::Exception& e )
    {
        context_.tracer()->warning( e.what() );
    }
    painter_.setLaserMaxRange( descr.maxRange );
}

QStringList
LaserScanner2dElement::contextMenu()
{
    QStringList s;
    s<<"Toggle Scan"<<"Toggle Points"<<"Toggle Walls"<<"Toggle Reflectors"<<"Toggle Filling Polygon"<<"Toggle Transparency";
    return s;
}

void
RangeScanner2dElement::getScannerInfo()
{
    // Subscribe directly to get geometry etc
    RangeScanner2dPrx scannerPrx;

    // Don't bother catching exceptions: they'll get caught higher up.

    orcaice::connectToInterfaceWithString( context_, scannerPrx, listener_.interfaceName() );
    
    RangeScanner2dDescription descr;
    descr = scannerPrx->getDescription();

    // we may get an exception if the laser is not mounted horizontal
    // we display a warning but paint it anyway.
    try
    {
        painter_.setOffset( descr.offset );
    }
    catch ( const orcaqgui::Exception& e )
    {
        context_.tracer()->warning( e.what() );
    }
    painter_.setLaserMaxRange( descr.maxRange );
}

QStringList
RangeScanner2dElement::contextMenu()
{
    QStringList s;
    s<<"Toggle Scan"<<"Toggle Points"<<"Toggle Walls"<<"Toggle Reflectors"<<"Toggle Filling Polygon"<<"Toggle Transparency";
    return s;
}

QStringList
PolarFeature2dElement::contextMenu()
{
    QStringList s;
    s<<"Toggle Laser Reflectors"<<"Toggle Corners";
    return s;
}

void
Localise3dElement::update()
{
    if (!haveGeometry_)
        tryToGetGeometry();
    
    // standard update as in IceStormElement
    if ( !IceStormElement<Localise3dPainter,
            orca::Localise3dData,
            orca::Localise3dPrx,
            orca::Localise3dConsumer,
            orca::Localise3dConsumerPrx>::needToUpdate() )
        return;

    assert( !listener_.buffer().isEmpty() );
    
    listener_.buffer().getAndPop( data_ );
    
    // first error-check.
    if ( data_.hypotheses.size() == 0 )
    {
        std::stringstream ss;
        ss << "Localise3dElement::update(): Interface " << listener_.interfaceName() << ": Localise3dData had zero hypotheses";
        throw orcaqgui::Exception( ss.str() );
    }

    data_.hypotheses[0].mean.p.x -= origin_.x;
    data_.hypotheses[0].mean.p.y -= origin_.y;

    painter_.setData( data_ );

    // custom update
    const orca::Pose3dHypothesis &h = orcaice::mlHypothesis( data_ );
    x_ = h.mean.p.x;
    y_ = h.mean.p.y;
    theta_ = h.mean.o.y;
}

void
Localise3dElement::actionOnConnection()
{   
    
    if (!isConnected_) return;
    
    tryToGetGeometry();
    
    paintInitialData<orca::Localise3dPrx, Localise3dPainter>
        ( context_, listener_.interfaceName(), painter_ );
}

void 
Localise3dElement::tryToGetGeometry()
{
    VehicleGeometryDescriptionPtr geom;
    
    try 
    {
        geom = listener_.proxy()->getVehicleGeometry();
        haveGeometry_=true;
    }
    catch ( std::exception &e)
    {
//         humanManager_->showStatusMsg(Error,"Exception when trying to get geometry: " + QString(e.what()) );
        cout << "Exception when trying to get geometry: " << e.what();
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
//         humanManager_->showStatusMsg(Warning, "Unknown platform type. Will paint a rectangle");
        VehicleGeometryCuboidDescriptionPtr cubGeom = VehicleGeometryCuboidDescriptionPtr::dynamicCast( geom );
        painter_.setTypeAndGeometry( PlatformTypeCubic, cubGeom->size.l, cubGeom->size.w );
        painter_.setOrigin( cubGeom->vehicleToGeometryTransform.p.x, cubGeom->vehicleToGeometryTransform.p.y, cubGeom->vehicleToGeometryTransform.o.y );
    }

}

QStringList
Localise3dElement::contextMenu()
{
    QStringList s;
    s<<"Toggle History"<<"Toggle Multi-Hypothesis";
    return s;
}

void
Localise3dElement::execute( int action )
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
