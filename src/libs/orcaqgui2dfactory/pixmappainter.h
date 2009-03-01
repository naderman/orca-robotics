/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAGUI_PIXMAP_PAINTER_H
#define ORCAGUI_PIXMAP_PAINTER_H

#include <vector>
#include <QPixmap>
#include <QMatrix>

#include <hydroqgui/hydroqgui.h>
#include <hydroqguielementutil/hydroqguielementutil.h>

namespace orcaqgui2d
{
    
// Data structure containing drawing information
struct PixmapData
{
    // Each pixel is a cell of size cellSize in [m]
    QSizeF cellSize;
    
    // Pixmap size in number of cells
    QSize mapSizePix;
    
    // Offset wrt the global coordinate system
    QPointF offset;
    
    // Three color channels as a flat representation of the pixels:
    // vectorIndex = y*mapSizePix.width()+x
    // WARNING: need to make sure that vector is of size ( mapSizePix.width()*mapSizePix.height() )
    std::vector<unsigned char> rgbR;
    std::vector<unsigned char> rgbG;
    std::vector<unsigned char> rgbB;
};

//! Class to paint pixmaps such as certainty grid maps and aerial maps.
//! Don't inherit from it but rather use as a member variable (composition).
//! Interface to it using PixmapData (constructor and/or setData).
//! @author Tobias Kaupp
class PixmapPainter
{
    
  public:
    PixmapPainter();
    PixmapPainter( PixmapData &pixmapData );
    ~PixmapPainter();
    
    void setData( PixmapData &pixmapData );

    void paint( QPainter *p );

    void toggleDisplayMap();
    int saveMap( const QString                       &fileName,
                 const QString                       &fileExtension,
                 hydroqguielementutil::IHumanManager *humanManager );
    
  private:
    
    bool updateWorldMatrix( const QMatrix & );
    bool updateWindowSize( const QSize & );
    void rescale();
    
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

    // record the offset so that we can check if it's changed and then update
    // if necessary  
    QPointF previousOffset_; 

};

} // namespace
  
#endif
