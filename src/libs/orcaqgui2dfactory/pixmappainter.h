/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCAGUI_PIXMAP_PAINTER_H
#define ORCAGUI_PIXMAP_PAINTER_H

#include <vector>
#include <QPixmap>
#include <QMatrix>

#include <orcaqgui2d/definitions2d.h>


namespace orcaqgui
{
    
struct PixmapData
{
    // OG cell size in [m]
    QSizeF cellSize;
    // OG map size in number of cells
    QSize mapSizePix;
    // origin of the bottomleft cnr of the map
    QPointF origin;
    // three color channels as a flat representation of the pixels
    std::vector<unsigned char> rgbR;
    std::vector<unsigned char> rgbG;
    std::vector<unsigned char> rgbB;
};
    
class IHumanManager; 

// Class to paint pixmaps such as certainty grid maps and aerial maps
class PixmapPainter
{
    
  public:
    PixmapPainter( int winMaxWidth=1000, int winMaxHeight=1000  );
    ~PixmapPainter();
    
    void setData( PixmapData &pixmapData );

    void paint( QPainter *p, int z );

    void clear();
    void toggleDisplayMap();
    int saveMap( const QString fileName, IHumanManager *humanManager );
    
  private:
    
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
    
    PixmapData data_;

    bool isDisplayMap_;

    bool haveMap_;
};

} // namespace
  
#endif
