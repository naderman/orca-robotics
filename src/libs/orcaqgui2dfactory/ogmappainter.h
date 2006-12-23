/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCAGUI_OGMAP_PAINTER_H
#define ORCAGUI_OGMAP_PAINTER_H

#include <orca/ogmap.h>
#include <orcaice/context.h>
#include <orcaqgui2d/definitions2d.h>
#include <orcaqgui2dfactory/pixmappainter.h>

namespace orcaqgui
{
    
class IHumanManager;  

    
typedef enum
{
    BITMAP,
    ICE_STREAM,
    NOT_SUPPORTED
} ImageFileType;

class OgMapPainter
{
    
  public:
    OgMapPainter();
    ~OgMapPainter();

    void paint( QPainter *p, int z );
    bool paintThisLayer( int z ) const { return z==Z_OG_MAP; }

    // Give the PixmapPainter the data relevant for painting
    void setData( const orca::OgMapData& data );
    
    void clear() {};
    void toggleDisplayMap() { pixmapPainter_->toggleDisplayMap(); };
    
    int saveMap( const orcaice::Context & context, const QString fileName, IHumanManager *humanManager );
    
  private:
      
    orca::OgMapData data_;
    PixmapPainter *pixmapPainter_;
    ImageFileType checkFileExtension( QString &fe, IHumanManager *humanManager );
};

} // namespace
  
#endif
