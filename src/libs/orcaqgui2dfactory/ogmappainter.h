/*
 * Orca-Robotics Project: Components for robotics
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAGUI_OGMAP_PAINTER_H
#define ORCAGUI_OGMAP_PAINTER_H

#include <orca/ogmap.h>
#include <orcaice/context.h>
#include <hydroqguielementutil/definitions2d.h>
#include <orcaqgui2dfactory/pixmappainter.h>

namespace orcaqgui2d
{

typedef enum
{
    BITMAP,
    NOT_SUPPORTED
} ImageFileType;

class OgMapPainter
{
public:

    void paint( QPainter *p, int z );
    bool paintThisLayer( int z ) const { return z==hydroqguielementutil::Z_OG_MAP; }

    // Give the PixmapPainter the data relevant for painting
    void setData( const orca::OgMapData& data );

    void clear() {};
    void toggleDisplayMap() { pixmapPainter_.toggleDisplayMap(); };

    int saveMap( const orcaice::Context   &context,
                 const QString            &fileName,
                 hydroqguielementutil::IHumanManager *humanManager );

    // paint on the pixmap
    void drawCircle ( const QPoint& center, qreal diameter );

private:

    orca::OgMapType mapType_;

    // Keep a persistent pixmapData_ store so we only have to allocate it once
    // TODO: does pixmapPainter_ has 2 more copies of the same data?
    PixmapData    pixmapData_;
    PixmapPainter pixmapPainter_;
    ImageFileType checkFileExtension( QString &fe, hydroqguielementutil::IHumanManager *humanManager );
};

} // namespace

#endif
