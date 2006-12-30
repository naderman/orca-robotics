/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef OGMAPS_COMBINED_ELEMENT_H
#define OGMAPS_COMBINED_ELEMENT_H

#include <vector>
#include <orcaqgui/icestormlistener.h>
#include <orcaqgui2d/guielement2d.h>
#include <orcaqgui2dfactory/connectutils.h>
#include <orcaqgui2dfactory/ogmapscombinedpainter.h>

namespace orcaqgui {
    
// A GuiElement which can combine several (at the moment two) ogmaps
// Subscribes to several (at the moment two) OgMap interfaces
// Author: Tobias Kaupp
class OgMapsCombinedElement : public GuiElement2d
{

public:
    OgMapsCombinedElement( const orcaice::Context  &context,
                           const QStringList       &proxyStrList);

    // inherited from guielement
    void update();
    bool needToUpdate();
    void paint( QPainter *p, int z ) 
        { painter_->paint( p, z ); };
    bool paintThisLayer( int z ) const
        { return painter_->paintThisLayer(z); }
    
    virtual bool isInGlobalCS() { return true; }
    virtual void actionOnConnection();
    virtual QStringList contextMenu();
    virtual void execute( int action );

private:
    orcaice::Context context_;
    std::vector<IceStormListener<
            orca::OgMapData,
            orca::OgMapPrx,
            orca::OgMapConsumer,
            orca::OgMapConsumerPrx>* > listeners_;
    OgMapsCombinedPainter* painter_;
    bool isConnected_;
};

}

#endif
