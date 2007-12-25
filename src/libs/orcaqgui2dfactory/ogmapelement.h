/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef OGMAPELEMENT_H
#define OGMAPELEMENT_H

#include <orcaqgui2dfactory/ogmappainter.h>
#include <orcaqgui2d/icestormelement.h>
#include <hydroqgui/hydroqgui.h>
#include <orcaqgui2dfactory/connectutils.h>

namespace orcaqgui2d {

class OgMapElement
    : public IceStormElement<OgMapPainter,
                             orca::OgMapData,
                             orca::OgMapPrx,
                             orca::OgMapConsumer,
                             orca::OgMapConsumerPrx>
{

public:
    OgMapElement( const orcaice::Context  &context,
                  const std::string       &proxyString,
                  hydroqgui::IHumanManager* humanManager );

    virtual bool isInGlobalCS() { return true; }
    virtual void actionOnConnection() 
    { 
        std::cout << "Paint initial data begin" << std::endl;
        paintInitialData<orca::OgMapPrx, OgMapPainter>
            ( context_, listener_.interfaceName(), painter_ );
        std::cout << "Paint initial data finished" << std::endl;
    }
    virtual QStringList contextMenu();
    virtual void execute( int action );

public:
    void saveMapAs();
    void saveMap();

private:
    OgMapPainter painter_;
    hydroqgui::IHumanManager *humanManager_;
    QString mapFileName_;
    bool mapFileNameSet_;
};

}

#endif
