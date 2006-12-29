/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef MULTIPLE_OGMAP_PAINTER_H
#define MULTIPLE_OGMAP_PAINTER_H

#include <orca/ogmap.h>
#include <orcaqgui2d/definitions2d.h>
#include <orcaqgui2dfactory/pixmappainter.h>


namespace orcaqgui
{

class MultipleOgMapPainter
{
  public:
    MultipleOgMapPainter();
    ~MultipleOgMapPainter();

    void paint( QPainter *p, int z );
    bool paintThisLayer( int z ) const { return z==Z_OG_MAP; }

    // Give the PixmapPainter the data relevant for painting
    void setData( const orca::OgMapData& data0, const orca::OgMapData& data1 );
    
    void toggleDisplayMap() { pixmapPainter_->toggleDisplayMap(); };
    
  private:
    PixmapPainter *pixmapPainter_;
    QImage *lookupTable_;

};

} // namespace
  
#endif
