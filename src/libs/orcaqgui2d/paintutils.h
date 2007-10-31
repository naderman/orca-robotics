/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#ifndef QORCAGUI_PAINT_UTILS_H
#define QORCAGUI_PAINT_UTILS_H

#include <QColor>
#include <QPolygonF>
#include <QPainter>

//
// Generally useful tools that are used by several 
// painters.
//
// Authors: Alex Brooks, Tobias Kaupp
//
namespace orcaqgui2d
{

//!
//! Class for remembering the path of a robot
//!
class PoseHistory
{
public:

    PoseHistory( double lineThickness=2.0 );

    // Add a point (units: metres)
    void addPoint( const double x, const double y );

    void paint( QPainter *p, const QColor &c );

private:

    QVector<QPolygonF> histories_;
    double lineThickness_;          // in pixel
    int totalPoints_;
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
                             const QColor   &colour, 
                             float     length, 
                             float     width, 
                             float     transparencyMultiplier=1.0, 
                             float     minLength=0.0, 
                             float     lineThickness=0.2 );

//! As above but paints a circular platform
void paintCylindricalPlatformPose( QPainter *p, 
                                   const QColor   &colour, 
                                   float     radius, 
                                   float     transparencyMultiplier=1.0, 
                                   float     minRadius=0.0, 
                                   float     lineThickness=0.2 );

//! Paints an ellipse for the position uncertainty, plus
//! a little wedge for the heading uncertainty, by calling
//! 'paintUncertaintyInfo' and 'paintCovarianceEllipse'
void paintUncertaintyInfo( QPainter     *p, 
                           const QColor &colour, 
                           float         pxx, 
                           float         pxy, 
                           float         pyy, 
                           float         ptt, 
                           float         length, 
                           float         lineThickness=0.2 );

//! Paints a little wedge of sidelength 'length' representing rotational uncertainty
void paintUncertaintyWedge( QPainter     *p, 
                            const QColor &colour, 
                            float         ptt, 
                            float         length, 
                            float         lineThickness=0.2 );

//! Paints 2-sigma ellipse for the position uncertainty
void paintCovarianceEllipse( QPainter     *p, 
                             const QColor &colour, 
                             float         pxx, 
                             float         pxy, 
                             float         pyy, 
                             float         lineThickness=0.2 );

//! Get a more transparent version of the given colour
//! transparencyMultiplier=1.0 gives the same colour.
QColor getTransparentVersion( const QColor &c, 
                              float         transparencyMultiplier=0.5 );

//! Get a darker version of the given colour
QColor getDarkVersion( const QColor &color );

//! Paint a waypoint with distance tolerance [m] as a circle and heading tolerance [deg] as a wedge around targetHeading [deg]
void paintWaypoint( QPainter     *p, 
                    const QColor &fillColor,
                    const QColor &drawColor,
                    int           targetHeading, 
                    float         distanceTolerance, 
                    int           headingTolerance );

}

#endif
