/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#ifndef HYDROQGUI_PAINT_UTILS_H
#define HYDROQGUI_PAINT_UTILS_H

#include <QColor>
#include <QPolygonF>
#include <QPainter>

//
// Generally useful tools that are used by several 
// painters.
//
// Authors: Alex Brooks, Tobias Kaupp
//
namespace hydroqguielementutil
{

//!
//! Class for remembering the path of a robot
//!
class PoseHistory
{
public:

    PoseHistory( double lineThickness=2.0,
                 int maxNumPoints=1000 );

    // Add a point (units: metres)
    void addPoint( const double x, const double y );

    void paint( QPainter *p, const QColor &c );

    // Max length measured in size of array
    void setMaxNumPoints( int maxNumPoints )
        { maxNumPoints_ = maxNumPoints; }
    int currentMaxNumPoints() const
        { return maxNumPoints_; }

private:

    QVector<QPolygonF> histories_;
    double lineThickness_;          // in pixel
    int totalPoints_;
    int maxNumPoints_;
};

////////////////////////////////////////////////////////////////////////////////

//!
//! Convenience class: calls painter->save() in constructor, 
//! and painter->restore() in destructor.
//!
//! Ensures that they always come in pairs, and provides 
//! safety in the presence of exceptions.
//!
class ScopedSaver {

public:
    ScopedSaver( QPainter *p )
        : p_(p)
        { p_->save(); }
    ~ScopedSaver()
        { p_->restore(); }
private:
    QPainter *p_;
};


//! Paint crosshairs with length of cross = 2 x radius
void paintOrigin( QPainter       *p, 
                  const QColor   &colour,
                  float           radius,
                  float           lineThickness=0.2 );

//! Paints a rectangular robot icon. All physical units are in meters. 
//! The painter needs to be translated and rotated before calling this function. 
//! The robot's geometry extends around the painter's current position. 
//! If minLength is specified, the robot is never painted smaller than this, independent of the zoom configuration.
void paintCubicPlatformPose( QPainter *p, 
                             float     length, 
                             float     width, 
                             float     transparencyMultiplier=1.0, 
                             float     minLength=0.0 );

//! As above but paints a circular platform
void paintCylindricalPlatformPose( QPainter *p, 
                                   float     radius, 
                                   float     transparencyMultiplier=1.0, 
                                   float     minRadius=0.0 );

//! Paints a little wedge of sidelength 'length' representing rotational uncertainty
void paintUncertaintyWedge( QPainter     *p, 
                            float         ptt, 
                            float         length );

//! Paints 2-sigma ellipse for the position uncertainty
void paintCovarianceEllipse( QPainter     *p, 
                             float         pxx, 
                             float         pxy, 
                             float         pyy );

//! Get a more transparent version of the given colour
//! opacity=1.0 gives the same colour.
QColor getTransparentVersion( const QColor &c, 
                              float         opacity=0.5 );

//! Get a darker version of the given colour
QColor getDarkVersion( const QColor &color );

//! Paint a waypoint with distance tolerance [m] as a circle and heading tolerance [deg/16] as a wedge around targetHeading [deg]
void paintWaypoint( QPainter     *p, 
                    const QColor &fillColor,
                    const QColor &drawColor,
                    int           targetHeading,      // Warning: Units are 16ths of degrees !
                    float         distanceTolerance, 
                    int           headingTolerance ); // Warning: Units are 16ths of degrees !

void paintArrow( QPainter *p );

//! Returns the scaling factor currently associated with the painter's worldMatrix.
//! The units are pixels/metre.
//! (Note that in general there's an x and a y scaling factor, but we assume that
//!  x and y are scaled equally).
double worldMatrixScale( QPainter *p );

}

#endif
