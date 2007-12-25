/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#ifndef ORCAGUI_WIFI_PAINTER_H
#define ORCAGUI_WIFI_PAINTER_H

#include <orca/wifi.h>
#include <orcaqgui2d/definitions2d.h>

// forward declarations
class QPainter;

namespace orcaqgui2d {

// Not painting anything into the view at the moment
class WifiPainter
{
  
  public:
    WifiPainter();
    ~WifiPainter();

    void setData( const orca::WifiData& wifiData );
    void setColor( QColor color ) { color_ = color; };
    void paint( QPainter *p, int z1 );
    bool paintThisLayer(int z) const { return z==Z_FOREGROUND; }
    void setUseTransparency( bool useTransparency ) { useTransparency_= useTransparency; };
    void clear();
    
  private:
    QColor color_;
    bool useTransparency_;
};

}

#endif
