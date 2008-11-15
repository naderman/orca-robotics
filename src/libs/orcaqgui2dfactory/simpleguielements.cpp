/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <orcaobj/miscutils.h>
#include "simpleguielements.h"

using namespace std;
using namespace orcaqgui2d;

namespace orcaqgui2d {

void
LaserScanner2dElement::getLaserInfo()
{
    // Subscribe directly to get geometry etc
    orca::LaserScanner2dPrx laserPrx;

    // Don't bother catching exceptions: they'll get caught higher up.

    orcaice::connectToInterfaceWithString( context_, laserPrx, listener_.interfaceName() );
    
    orca::RangeScanner2dDescription descr;
    descr = laserPrx->getDescription();

    // we may get an exception if the laser is not mounted horizontal
    // we display a warning but paint it anyway.
    try
    {
        painter_.setOffset( descr.offset );
    }
    catch ( const std::exception& e )
    {
        context_.tracer().warning( e.what() );
    }
    painter_.setLaserMaxRange( descr.maxRange );
}

void
PolarFeature2dElement::getExtractorInfo()
{
    // Subscribe directly to get geometry etc
    orca::PolarFeature2dPrx prx;

    // Don't bother catching exceptions: they'll get caught higher up.

    orcaice::connectToInterfaceWithString( context_, prx, listener_.interfaceName() );
    
    orca::PolarFeature2dDescription descr = prx->getDescription();

    // we may get an exception if the sensor is not mounted horizontal
    // we display a warning but paint it anyway.
    try
    {
        painter_.setOffset( descr.offset );
    }
    catch ( const std::exception& e )
    {
        context_.tracer().warning( e.what() );
    }
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
    orca::RangeScanner2dPrx scannerPrx;

    // Don't bother catching exceptions: they'll get caught higher up.

    orcaice::connectToInterfaceWithString( context_, scannerPrx, listener_.interfaceName() );
    
    orca::RangeScanner2dDescription descr;
    descr = scannerPrx->getDescription();

    // we may get an exception if the laser is not mounted horizontal
    // we display a warning but paint it anyway.
    try
    {
        painter_.setOffset( descr.offset );
    }
    catch ( const std::exception& e )
    {
        context_.tracer().warning( e.what() );
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

void
Localise3dElement::update()
{
    if (!haveGeometry_)
        tryToGetGeometry();
    
    // standard update as in IceStormElement2d
    if ( !orcaqguielementutil::IceStormElement2d<Localise3dPainter,
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
        throw hydroqgui::Exception( ERROR_INFO, ss.str() );
    }

    data_.hypotheses[0].mean.p.x -= origin_.x;
    data_.hypotheses[0].mean.p.y -= origin_.y;

    painter_.setData( data_ );

    // custom update
    const orca::Pose3dHypothesis &h = orcaobj::mlHypothesis( data_ );
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
    orca::VehicleGeometryDescriptionPtr geom;
    
    try 
    {
        geom = listener_.proxy()->getDescription();
        haveGeometry_=true;
    }
    catch ( std::exception &e)
    {
//         humanManager_->showStatusMsg(hydroqguielementutil::IHumanManager::Error,"Exception when trying to get geometry: " + QString(e.what()) );
        cout << "Exception when trying to get geometry: " << e.what();
    }
    
    if (!haveGeometry_) {
        painter_.setTypeAndGeometry(PlatformTypeCubic, 0.5, 0.4 );
        painter_.setOrigin( 0.0, 0.0, 0.0 );
        return;
    }
    
    if (geom->type==orca::VehicleGeometryCuboid)
    {
        orca::VehicleGeometryCuboidDescriptionPtr geom = orca::VehicleGeometryCuboidDescriptionPtr::dynamicCast( geom );
        painter_.setTypeAndGeometry( PlatformTypeCubic, geom->size.l, geom->size.w );
        painter_.setOrigin( geom->platformToGeometryTransform.p.x, geom->platformToGeometryTransform.p.y, geom->platformToGeometryTransform.o.y );
    }
    else if (geom->type==orca::VehicleGeometryCylindrical)
    {
        orca::VehicleGeometryCylindricalDescriptionPtr cylGeom = orca::VehicleGeometryCylindricalDescriptionPtr::dynamicCast( geom );
        painter_.setTypeAndGeometry( PlatformTypeCylindrical, cylGeom->radius );
        painter_.setOrigin( cylGeom->platformToGeometryTransform.p.x, cylGeom->platformToGeometryTransform.p.y, cylGeom->platformToGeometryTransform.o.y );
    }
    else
    {
//         humanManager_->showStatusMsg(hydroqguielementutil::IHumanManager::Warning, "Unknown platform type. Will paint a rectangle");
        orca::VehicleGeometryCuboidDescriptionPtr cubGeom = orca::VehicleGeometryCuboidDescriptionPtr::dynamicCast( geom );
        painter_.setTypeAndGeometry( PlatformTypeCubic, cubGeom->size.l, cubGeom->size.w );
        painter_.setOrigin( cubGeom->platformToGeometryTransform.p.x, cubGeom->platformToGeometryTransform.p.y, cubGeom->platformToGeometryTransform.o.y );
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
        throw hydroqgui::Exception( ERROR_INFO, "execute(): What the hell? bad action." );
        break;
    }
    }
}


}
