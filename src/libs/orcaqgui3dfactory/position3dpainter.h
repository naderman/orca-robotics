/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCAGUI3D_POSITION3D_PAINTER_H
#define ORCAGUI3D_POSITION3D_PAINTER_H

#include <QColor>
#include <QGLWidget>
#include <orca/position3d.h>
#include <orcaqgui3dfactory/objectmodel.h>

namespace orcaqgui3d
{

/*!
  @brief LaserPainter paints laser scans
*/
class Position3dPainter
{
  
  public:
    Position3dPainter( const char *objectModelFilename );
    ~Position3dPainter();

    // void setGeometry( orca::RangeScanner2dGeometryPtr &geom );
    void setData( const orca::Position3dDataPtr &scan );
    
    void paint( QGLWidget *p );

    void clear();
    
    // void execute( int action );
    
    //void setColor( QColor color );
    //void setFocus( bool inFocus );

    const orca::Frame3d pose() const { return data_->pose; }

  private:

    orca::Position3dDataPtr data_;

    // This thing loads a 3d model from a file
    ObjectModel *objectModel_;
};

} // namespace

#endif
