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

#include <QPixmap>
#include <QMatrix>

#include <orca/ogmap.h>
#include <orcaice/context.h>
#include <orcaqgui2d/definitions2d.h>
#include <orcaqgui2dfactory/ogmappainter.h>


namespace orcaqgui
{

class MultipleOgMapPainter
{
  public:
    MultipleOgMapPainter( int winMaxWidth=1000, int winMaxHeight=1000  );
    ~MultipleOgMapPainter();

    void paint( QPainter *p, int z );
    bool paintThisLayer( int z ) const { return z==Z_OG_MAP; }

    // Copy the orca map structure into a QPixmap
    void setData( const orca::OgMapDataPtr & data0, const orca::OgMapDataPtr & data1 );
    void clear();
    void toggleDisplayMap();
    
  private:
      
    orca::OgMapDataPtr data_;
    
    bool updateWorldMatrix( const QMatrix & );
    bool updateWindowSize( const QSize & );
    void rescale();
    void reset();
    
    // Maximum widget size in pixels (WorldView)
    QSize winMaxSize_;
    
    // Unscaled map storage
    QPixmap qMap_;
       
    // qMap_ scaled and cropped to current window size
    QPixmap mapWin_;
    
    // Current widget size in pixels (WorldView)
    QSize winSize_;
    
    // Current meters-to-window transformation matrix
    QMatrix m2win_;
    
    // Current map-to-window transformation matrix
    QMatrix map2win_;
    
    // OG cell size in [m]
    QSizeF cellSize_;
    
    // OG map size in number of cells
    QSize mapSizePix_;
    // OG map size in [m]
    QSizeF mapSizeM_;

    // origin of the bottomleft cnr of the map,
    QPointF origin_;
    // in map coord system, in cells
    int originX_;
    int originY_;

    bool isDisplayMap_;

    bool haveMap_;

};

} // namespace
  
#endif
