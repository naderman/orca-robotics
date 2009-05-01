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

#include <orcaqgui3dfactory/ogmappainter.h>
#include <orcaqgui3d/icestormelement3d.h>

namespace orcaqgui3d {

class OgMapElement
    : public orcaqgui3d::IceStormElement3d<OgMapPainter,
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

    osg::Node *osgNode() const { return painter_.osgNode(); }

private:
    OgMapPainter painter_;
    hydroqguielementutil::IHumanManager &humanManager_;
    QString mapFileName_;
    bool mapFileNameSet_;
};

}

#endif
