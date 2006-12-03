/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <orcaice/orcaice.h>

#include "simpleguielements.h"

#include <QTranslator>
#include <QMenu>
#include <QMenuBar>
#include <QMainWindow>

using namespace std;
using namespace orca;
using namespace orcaqgui;

namespace orcaqgui {

void
LaserScanner2dElement::getLaserInfo()
{
    // Subscribe directly to get geometry etc
    LaserScanner2dPrx laserPrx;

    // Don't bother catching exceptions: they'll get caught higher up.

    orcaice::connectToInterfaceWithString( context_, laserPrx, listener_.interfaceName() );
    
    RangeScanner2dDescriptionPtr descr = new RangeScanner2dDescription;
    descr = laserPrx->getDescription();

    painter_.setOffset( descr->offset );
    painter_.setLaserMaxRange( descr->maxRange );
}

QStringList
LaserScanner2dElement::contextMenu()
{
    QStringList s;
    s<<"Toggle Scan"<<"Toggle Walls"<<"Toggle Reflectors"<<"Toggle Filling Polygon"<<"Toggle Transparency";
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
Localise2dElement::update()
{
    // standard update as in IceStormElement
    if ( !IceStormElement<Localise2dPainter,
            orca::Localise2dData,
            orca::Localise2dDataPtr,
            orca::Localise2dPrx,
            orca::Localise2dConsumer,
            orca::Localise2dConsumerPrx>::needToUpdate() )
        return;

    assert( !listener_.buffer().isEmpty() );
    
    listener_.buffer().getAndPop( data_ );
    painter_.setData( data_ );

    // custom update, but first error-check.
    if ( data_->hypotheses.size() == 0 )
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
