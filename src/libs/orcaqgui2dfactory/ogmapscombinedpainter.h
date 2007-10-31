/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef OGMAPS_COMBINED_PAINTER_H
#define OGMAPS_COMBINED_PAINTER_H

#include <orca/ogmap.h>
#include <orcaqgui2d/definitions2d.h>
#include <orcaqgui2dfactory/pixmappainter.h>


namespace orcaqgui2d
{

// A painter which can overlay several (at the moment two) OgMaps
// Author: Tobias Kaupp
class OgMapsCombinedPainter
{
  public:
    OgMapsCombinedPainter( const orcaice::Context  &context );
    ~OgMapsCombinedPainter();

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
