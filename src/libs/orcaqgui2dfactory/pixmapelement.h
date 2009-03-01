/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef PIXMAPELEMENT_H
#define PIXMAPELEMENT_H

#include <orcaqgui2dfactory/pixmapbackgroundpainter.h>
#include <orcaqguielementutil/icestormelement2d.h>
#include <hydroqgui/hydroqgui.h>
#include <orcaqgui2dfactory/connectutils.h>

namespace orcaqgui2d {

class PixMapElement
    : public orcaqguielementutil::IceStormElement2d<PixMapBackgroundPainter,
                                             orca::PixMapData,
                                             orca::PixMapPrx,
                                             orca::PixMapConsumer,
                                             orca::PixMapConsumerPrx>
{

public:
    PixMapElement( const orcaice::Context  &context,
                  const std::string       &proxyString,
                  hydroqguielementutil::IHumanManager* humanManager );

    virtual bool isInGlobalCS() { return true; }
    virtual void actionOnConnection() 
    { 
        std::cout << "Paint initial data begin" << std::endl;
        paintInitialData<orca::PixMapPrx, PixMapBackgroundPainter>
            ( context_, listener_.interfaceName(), painter_ );
            std::cout << "Paint initial data finished" << std::endl;
    }
    virtual QStringList contextMenu();
    virtual void execute( int action );

public:
    void saveMapAs();
    void saveMap();

private:
    PixMapBackgroundPainter painter_;
    hydroqguielementutil::IHumanManager *humanManager_;
    QString mapFileName_;
    bool mapFileNameSet_;
};

}

#endif
