#if 0
/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAGUI3D_POSITION3D_PAINTER_H
#define ORCAGUI3D_POSITION3D_PAINTER_H

#include <QColor>
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
    void setData( const orca::Position3dData& obj );
    
    void paint( QGLWidget *p );

    void clear();
    
    // void execute( int action );
    
    //void setColor( QColor color );
    //void setFocus( bool inFocus );

    const orca::Frame3d pose() const { return data_.pose; }

  private:

    orca::Position3dData data_;

    // This thing loads a 3d model from a file
    ObjectModel *objectModel_;
};

} // namespace

#endif
#endif

