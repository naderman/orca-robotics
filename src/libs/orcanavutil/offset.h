/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef ORCA_NAV_UTIL_OFFSET__H
#define ORCA_NAV_UTIL_OFFSET__H

#include <iostream>

//
// A collection of small utility functions for doing
// navigation-related things.
//
// @author Alex Brooks, Tobias Kaupp
//

namespace orcanavutil {

    //! Simple class for representing offsets
    class Offset {
    public:
        Offset() {}
        Offset(double x, double y, double theta)
            { this->x=x; this->y=y; this->theta=theta; }
        double x;
        double y;
        double theta;
    };

    std::ostream &operator<<( std::ostream &s, const Offset &o );
    std::string toString( const Offset &o );

    //!
    //! Adds an offset (in local coordinates) onto a pose, modifying the pose.
    //!
    //! Adds the linear bit, then applies the rotation. 
    //!
    void addPoseOffset( double &poseX,
                        double &poseY,
                        double &poseT,
                        double offsetX,
                        double offsetY,
                        double offsetT,
                        bool  normaliseHeading );

    //!
    //! Adds an offset (in local coordinates) onto a pose, producing a result
    //!
    //! Adds the linear bit, then applies the rotation. 
    //!
    void addPoseOffset( double startX,
                        double startY,
                        double startT,
                        double offsetX,
                        double offsetY,
                        double offsetT,
                        double &resultX,
                        double &resultY,
                        double &resultT,
                        bool   normaliseHeading );

    //! normalise to [-pi,pi).
    void normaliseAngle( double &angle );

    //!
    //! We have some path, defined by an offset.
    //! This removes a chunk of it from the front.
    //!
    void subtractInitialOffset( double &totalOffsetX,
                                double &totalOffsetY,
                                double &totalOffsetTheta,
                                double  initialOffsetX,
                                double  initialOffsetY,
                                double  initialOffsetTheta );

    //!
    //! We have some path, defined by an offset.
    //! This removes a chunk of it from the front.
    //!
    void subtractInitialOffset( double  totalOffsetX,
                                double  totalOffsetY,
                                double  totalOffsetTheta,
                                double  initialOffsetX,
                                double  initialOffsetY,
                                double  initialOffsetTheta,
                                double &resultX,
                                double &resultY,
                                double &resultTheta );
       
    //! Transforms a point (xIn,yIn) in one coordinate system
    //! to a point (xOut,yOut) in another coordinate system
    //! using the offset between the two coordinate systems
    //! (translation: offsetX/Y, rotation:offsetT)
    void transformPoint2d( double  xIn, 
                           double  yIn,
                           double  offsetX,
                           double  offsetY,
                           double  offsetT,
                           double &xOut,
                           double &yOut);
            
}

#endif
