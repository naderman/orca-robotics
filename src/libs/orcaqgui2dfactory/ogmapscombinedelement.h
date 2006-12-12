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
#include <orcaqgui2dfactory/ogmappainter.h>
#include <orcaqgui/icestormlistener.h>
#include <orcaqgui2d/guielement2d.h>
#include <orcaqgui/ihumanmanager.h>
#include <orcaqgui2dfactory/connectutils.h>

namespace orcaqgui {
    
class OgMapsCombinedElement : public GuiElement2d
{

public:
    OgMapsCombinedElement( const orcaice::Context  &context,
                           const QStringList       &proxyStrList,
                           IHumanManager*           humanManager);

    // inherited from guielement
    void update();
    bool needToUpdate();
    void paint( QPainter *p, int z ) 
        { painter_.paint( p, z ); };
    bool paintThisLayer( int z ) const
        { return painter_.paintThisLayer(z); }
    
    virtual bool isInGlobalCS() { return true; }
    virtual void actionOnConnection();
//     { 
//         std::cout << "Paint initial data begin" << std::endl;
//         paintInitialData<orca::OgMapPrx, OgMapPainter>
//             ( context_, listener_.interfaceName(), painter_ );
//             std::cout << "Paint initial data finished" << std::endl;
//     }
    virtual QStringList contextMenu();
    virtual void execute( int action );

public:
    void saveMapAs();
    void saveMap();

private:
    std::vector<IceStormListener<orca::OgMapData,orca::OgMapDataPtr,orca::OgMapPrx,orca::OgMapConsumer,orca::OgMapConsumerPrx>* > listeners_;
    OgMapPainter painter_;
    IHumanManager *humanManager_;
    QString mapFileName_;
    bool mapFileNameSet_;
    orcaice::Context context_;
    orca::OgMapDataPtr totalMap_;
    void fuse( orca::OgMapDataPtr data);
    double                           timeoutMs_;
    bool                             isConnected_;
};

}

#endif
