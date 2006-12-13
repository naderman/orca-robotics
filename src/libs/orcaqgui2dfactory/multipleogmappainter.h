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
    MultipleOgMapPainter( int winMaxWidth=1000, int winMaxHeight=1000  );
    ~MultipleOgMapPainter();

    void paint( QPainter *p, int z ) { pixmapPainter_->paint( p,z); };
    bool paintThisLayer( int z ) const { return z==Z_OG_MAP; }

    // Copy the orca map structure into a QPixmap
    void setData( const orca::OgMapDataPtr & data0, const orca::OgMapDataPtr & data1 );
    void clear() { pixmapPainter_->clear(); };
    void toggleDisplayMap() { pixmapPainter_->toggleDisplayMap(); };
    
  private:
    PixmapPainter *pixmapPainter_;

};

} // namespace
  
#endif
