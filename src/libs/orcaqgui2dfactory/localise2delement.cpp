/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */


#include <orcanavutil/offset.h>
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
    
    humanManager_->showStatusMsg(Information, "Got Localise2d description");
    gotDescription_ = true;    
    
    paintInitialData<orca::Localise2dPrx, Localise2dPainter>
        ( context_, listener_.interfaceName(), painter_ );
}

void
Localise2dElement::update()
{
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
    
    orcanavutil::transformPoint2d( h.mean.p.x, h.mean.p.y, localiseOff_.p.x, localiseOff_.p.y, localiseOff_.o, x_, y_);
    theta_ = h.mean.o + localiseOff_.o;
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
