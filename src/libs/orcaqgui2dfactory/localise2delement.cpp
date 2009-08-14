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
#include <orcaobj/localise2d.h>
#include "localise2delement.h"
#include <QInputDialog>

using namespace std;

namespace orcaqgui2d {

void
Localise2dElement::actionOnConnection()
{
    if (!isConnected_) return;

    tryToGetGeometry();
}

void 
Localise2dElement::tryToGetGeometry()
{
    orca::VehicleGeometryDescriptionPtr geom;
    
    if ( listener_.proxy()==0 )
    {
        humanManager_->showStatusError("Localise2dElement::tryToGetGeometry(): listener_.proxy()==0");
    }
    try 
    {
        geom = listener_.proxy()->getDescription();
        if ( geom == 0 )
        {
            throw gbxutilacfr::Exception( ERROR_INFO, "Localise2dElement::tryToGetGeometry(): got NULL vehicle geometry from remote interface!" );
        }
        haveGeometry_ = true;
    }
    catch ( std::exception &e)
    {
        humanManager_->showStatusError( "Localise2dElement::tryToGetGeometry(): Exception when trying to get geometry: " + QString(e.what()) );
    }
    
    if (!haveGeometry_) 
    {
        humanManager_->showStatusWarning( "Localise2dElement::tryToGetGeometry(): couldn't get geometry.  Assuming point vehicle." );
        const double length = 1e-3, width = 1e-3;
        painter_.setTypeAndGeometry(PlatformTypeCubic, length, width );
        painter_.setOrigin( 0.0, 0.0, 0.0 );
        return;
    }
    
    if (geom->type==orca::VehicleGeometryCuboid)
    {
        orca::VehicleGeometryCuboidDescriptionPtr cuboidGeom = orca::VehicleGeometryCuboidDescriptionPtr::dynamicCast( geom );
        painter_.setTypeAndGeometry( PlatformTypeCubic, cuboidGeom->size.l, cuboidGeom->size.w );
        painter_.setOrigin( cuboidGeom->platformToGeometryTransform.p.x, cuboidGeom->platformToGeometryTransform.p.y, cuboidGeom->platformToGeometryTransform.o.y );
    }
    else if (geom->type==orca::VehicleGeometryCylindrical)
    {
        orca::VehicleGeometryCylindricalDescriptionPtr cylGeom = orca::VehicleGeometryCylindricalDescriptionPtr::dynamicCast( geom );
        painter_.setTypeAndGeometry( PlatformTypeCylindrical, cylGeom->radius );
        painter_.setOrigin( cylGeom->platformToGeometryTransform.p.x, cylGeom->platformToGeometryTransform.p.y, cylGeom->platformToGeometryTransform.o.y );
    }
    else
    {
        humanManager_->showStatusWarning("Localise2dElement::Unknown platform type. Will paint a rectangle");
        orca::VehicleGeometryCuboidDescriptionPtr cubGeom = orca::VehicleGeometryCuboidDescriptionPtr::dynamicCast( geom );
        painter_.setTypeAndGeometry( PlatformTypeCubic, cubGeom->size.l, cubGeom->size.w );
        painter_.setOrigin( cubGeom->platformToGeometryTransform.p.x, cubGeom->platformToGeometryTransform.p.y, cubGeom->platformToGeometryTransform.o.y );
    }
}

void
Localise2dElement::update()
{
//     if (!haveGeometry_)
//     {
//         cout<<"TRACE(localise2delement.cpp): "<<__func__<<": calling tryToGetGeometry()" << endl;
//         tryToGetGeometry();
//     }
    
    // standard update as in IceStormGuiElement2d
    if ( !orcaqguielementutil::IceStormGuiElement2d<Localise2dPainter,
            orca::Localise2dData,
            orca::Localise2dPrx,
            orca::Localise2dConsumer,
            orca::Localise2dConsumerPrx>::needToUpdate() )
        return;

    assert( !listener_.store().isEmpty() );
    
    listener_.store().get( data_ );
    painter_.setData( data_ );

    // custom update, but first error-check.
    if ( data_.hypotheses.size() == 0 )
    {
        std::stringstream ss;
        ss << "Localise2dElement::update(): Interface " << listener_.interfaceName() << ": Localise2dData had zero hypotheses";
        throw hydroqgui::Exception( ERROR_INFO, ss.str() );
    }
    const orca::Pose2dHypothesis &h = orcaobj::mlHypothesis( data_ );
    
    x_ = h.mean.p.x;
    y_ = h.mean.p.y;
    theta_ = h.mean.o;
}

QStringList
Localise2dElement::contextMenu()
{
    QStringList s;
    s<<"Toggle History"<<"Set Max Num History Points"<<"Toggle Multi-Hypothesis"<<"Toggle Uncertainty";
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
            bool ok=false;
            int maxNumHistoryPoints = QInputDialog::getInteger( 
                NULL,
                "SetMaxNumHistoryPoints",
                "Enter max num history points",
                painter_.currentMaxNumHistoryPoints(),
                0,
                2147483647, // default from Qt docs
                1,
                &ok );
            if ( ok )
                painter_.setMaxNumHistoryPoints( maxNumHistoryPoints );
            break;
        }
        case 2 :
        {
            painter_.toggleMultiHypothesis();
            break;
        }
        case 3 :
        {
            painter_.toggleUncertainty();
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
