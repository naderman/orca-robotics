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
#include <orcaqguielementutil/icestormelement2d.h>

namespace orcaqgui2d {

class OgMapElement
    : public orcaqguielementutil::IceStormElement2d<OgMapPainter,
                             orca::OgMapData,
                             orca::OgMapPrx,
                             orca::OgMapConsumer,
                             orca::OgMapConsumerPrx>
{

public:
    OgMapElement( const orcaice::Context  &context,
                  const std::string       &proxyString,
                  hydroqguielementutil::IHumanManager* humanManager );

    virtual bool isInGlobalCS() { return true; }
    virtual QStringList contextMenu();
    virtual void execute( int action );

public:
    void saveMapAs();
    void saveMap();

private:
    OgMapPainter painter_;
    hydroqguielementutil::IHumanManager *humanManager_;
    QString mapFileName_;
    bool mapFileNameSet_;
};

}

#endif
