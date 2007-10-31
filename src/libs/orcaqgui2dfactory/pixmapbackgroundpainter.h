/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAGUI_PIXMAP_BACKGROUND_PAINTER_H
#define ORCAGUI_PIXMAP_BACKGROUND_PAINTER_H

#include <orca/pixmap.h>
#include <orcaice/context.h>
#include <orcaqgui2d/definitions2d.h>
#include <orcaqgui2dfactory/pixmappainter.h>

namespace orcaqgui2d
{
    
class orcaqgui::IHumanManager;  

class PixMapBackgroundPainter
{
    
  public:
    PixMapBackgroundPainter();
    ~PixMapBackgroundPainter();

    void paint( QPainter *p, int z );
    bool paintThisLayer( int z ) const { return z==Z_BACKGROUND; }

    // Give the PixmapPainter the data relevant for painting
    void setData( const orca::PixMapData& data );
    
    void clear() {};
    void toggleDisplayMap() { pixmapPainter_->toggleDisplayMap(); };
    
    int saveMap( const orcaice::Context & context, const QString fileName, orcaqgui::IHumanManager *humanManager );
    
  private:
    PixmapPainter *pixmapPainter_;
    bool checkFileExtension( QString &fe, orcaqgui::IHumanManager *humanManager );
};

} // namespace
  
#endif
