/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef ORCA_NAV_UTIL__H
#define ORCA_NAV_UTIL__H

#include <iostream>

//
// A collection of small utility functions for doing
// navigation-related things.
//
// @author Alex Brooks
//

namespace orcanavutil {

    //! Simple class for representing offsets
    class Offset {
    public:
        Offset() 
            { x=0.0; y=0.0; theta=0.0; }
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
    //! This removes a chunk of it fron the front.
    //!
    void subtractInitialOffset( double &totalOffsetX,
                                double &totalOffsetY,
                                double &totalOffsetTheta,
                                double  initialOffsetX,
                                double  initialOffsetY,
                                double  initialOffsetTheta );

    //!
    //! We have some path, defined by an offset.
    //! This removes a chunk of it fron the front.
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
}

#endif
