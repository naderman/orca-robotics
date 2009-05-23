/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAGUI_RANGERARRAY_PAINTER_H
#define ORCAGUI_RANGERARRAY_PAINTER_H

#include <QColor>
#include <QPolygonF>
#include <orca/rangerarray.h>
#include <hydroqguielementutil/definitions2d.h>

class QPainter;

namespace orcaqgui2d
{

/*!
  @brief Paints ranger returns.
*/
class RangerArrayPainter
{
  
  public:
    // -1 means 'default'
    RangerArrayPainter( QColor outlineColor=Qt::blue,
			float  outlineThickness=-1,
			float  brightReturnWidth=0.2 );

    void setDescription( orca::RangerArrayDescription &description );
    void setData( const orca::RangerArrayDataPtr &scan );
    
    void paint( QPainter *p, int z );
    bool paintThisLayer(int z) const {return z==hydroqguielementutil::Z_RANGERARRAY;}

    void clear();
    
    void execute( int action );
    
    void setFocus( bool inFocus );

  private:

    std::vector<float>         ranges_;

    // Data for every ray
    struct RANGERDATA
    {
        // Scan (set of points for drawing the scan outline)
        QPolygonF qScan;

        // The actual return (doesn't include a point at the origin)
	QPolygonF qReturn;

	// The return arc (to make it more visible)
	QPolygonF qReturnArc;
    };
    std::vector<RANGERDATA> rangers_;

    bool isDisplayScan_;
    bool isDisplayPoints_;
    bool isFilledPolygon_;
    QColor outlineColor_;
    QColor fillColor_;
    QColor basisColor_;
    float  outlineThickness_;

    orca::RangerArrayDescription description_;

};

} // namespace

#endif
