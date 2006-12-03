/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCAGUI_QGRAPHICS2D_PAINTER_H
#define ORCAGUI_QGRAPHICS2D_PAINTER_H

#include <QColor>
#include <orca/qgraphics2d.h>
#include <QPicture>

class QPainter;

namespace orcaqgui
{

/*!
  \brief QGraphics2dPainter
  \author Alex Brooks
*/
class QGraphics2dPainter
{
  
  public:
    QGraphics2dPainter();
    ~QGraphics2dPainter();

    void setData( const orca::QGraphics2dDataPtr &data );
    
    void paint( QPainter *p, int z );
    bool paintThisLayer(int z) const;

    void clear();
    
    bool isInGlobalCS() const { return data_->isInGlobalCS; }

  private:

    orca::QGraphics2dDataPtr data_;    
    QPicture qPicture_;
};

} // namespace

#endif
