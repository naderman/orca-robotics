/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
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
// \author Alex Brooks
//
namespace orcaqgui
{

//!
//! class for remembering the path of a robot
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

    ////////////////////////////////////////

    // Minimum change required for the addition of a new point
    static const double _historyMinDistance = 0.250;    
    // Maximum change to qualify as a new point
    // This is to prevent 'teleporting'
    static const double _historyMaxDistance = 8.0;

    // Limit on the size of the history array
    static const int _historyMaxLength = 1000;
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


////////////////////////////////////////////////////////////////////////////////
// Nota Bene: All of these function draw whatever they're gonna draw _AT_THE_ORIGIN_!!
//            If you want them somewhere else, pre-rotate/translate the painter.
// Tobi: all functions that have the m2win matrix as an argument can't get it from the painter
//       because the painter is usually rotated before the function call. 
//       It is used to draw in pixel size rather than in meters. Useful for iconic
//       views, e.g. platform positions.

//! Paint crosshairs
//! Warning: Don't rotate painter before calling this function
void paintOrigin( QPainter *p, QColor colour );

//! Paint a robot icon, painted with fixed pixel sizes, independent of zoom configuration.
//! m2win is the worldmatrix BEFORE any painter rotation
void paintPlatformPose( QMatrix &m2win, QPainter *p, QColor colour, float transparencyMultiplier=1.0 );

//!
//! Paints an ellipse for the position uncertainty, plus
//! a little wedge for the heading uncertainty, by calling
//! 'paintUncertaintyInfo' and 'paintCovarianceEllipse'
//! m2win is the worldmatrix BEFORE any painter rotation.
void paintUncertaintyInfo( QMatrix &m2win, QPainter *p, QColor colour, float thetaMean, float pxx, float pxy, float pyy, float ptt );

//! Paints a little wedge representing rotational uncertainty
//! m2win is the worldmatrix BEFORE any painter rotation
void paintUncertaintyWedge( QMatrix &m2win, QPainter *p, QColor colour, float thetaMean, float ptt );

//! Paints 2-sigma ellipse for the position uncertainty
//! m2win is the worldmatrix BEFORE any painter rotation
void paintCovarianceEllipse( QMatrix &m2win, QPainter *p, QColor colour, float pxx, float pxy, float pyy );

//! Get a more transparent version of the given colour
//! transparencyMultiplier=1.0 gives the same colour.
QColor getTransparentVersion( const QColor &c, float transparencyMultiplier=0.5 );

//! Paint a waypoint with distance tolerance [m] as a circle and heading tolerance [deg] as a wedge around targetHeading [deg]
void paintWaypoint( QPainter *p, 
                    QColor &fillColor,
                    QColor &drawColor,
                    int targetHeading, 
                    float distanceTolerance, 
                    int headingTolerance );

}

#endif
