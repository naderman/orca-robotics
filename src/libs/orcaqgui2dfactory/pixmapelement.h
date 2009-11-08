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
#include <orcaqguielementutil/icestormguielement2d.h>

namespace orcaqgui2d {

class PixMapElement
    : public orcaqguielementutil::IceStormGuiElement2d<PixMapBackgroundPainter,
                                             orca::PixMapData,
                                             orca::PixMapPrx,
                                             orca::PixMapConsumer,
                                             orca::PixMapConsumerPrx>
{

public:
    PixMapElement( const hydroqguielementutil::GuiElementInfo &guiElementInfo,
                   const orcaice::Context                     &context );

    virtual bool isInGlobalCS() { return true; }
    virtual QStringList contextMenu();
    virtual void execute( int action );

public:
    void saveMapAs();
    void saveMap();

private:
    PixMapBackgroundPainter painter_;
    QString mapFileName_;
    bool mapFileNameSet_;
};

}

#endif
