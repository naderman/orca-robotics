/*
 * Orca-Robotics Project: Components for robotics
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef OGMAPELEMENT_H
#define OGMAPELEMENT_H

#include <orcaqgui2dfactory/ogmappainter.h>
#include <orcaqguielementutil/icestormguielement2d.h>

namespace hydroqguielementutil {
    class MouseEventManager;
    class IHumanManager;
}

namespace orcaqgui2d {

class OgMapElement
    : public orcaqguielementutil::IceStormGuiElement2d<OgMapPainter,
                             orca::OgMapData,
                             orca::OgMapPrx,
                             orca::OgMapConsumer,
                             orca::OgMapConsumerPrx>
{

public:
    OgMapElement( const hydroqguielementutil::GuiElementInfo &guiElementInfo,
                  const orcaice::Context  &context );

    // from GuiElement
    virtual bool isInGlobalCS() { return true; }
    virtual QStringList contextMenu();
    virtual void execute( int action );

    virtual void noLongerMouseEventReceiver();
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseMoveEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);
    virtual void mouseDoubleClickEvent(QMouseEvent *e);

    // context menu implementations
    void startPainting();
    void stopPainting();
    void saveMapAs();
    void saveMap();

private:
    OgMapPainter painter_;
    QString mapFileName_;
    bool mapFileNameSet_;
    bool ownMouseEvents_;
};

}

#endif
