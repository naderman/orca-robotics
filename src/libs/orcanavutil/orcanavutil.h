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
        Offset(float x, float y, float theta)
            { this->x=x; this->y=y; this->theta=theta; }
        float x;
        float y;
        float theta;
    };

    std::ostream &operator<<( std::ostream &s, const Offset &o );
    std::string toString( const Offset &o );

    //!
    //! Adds an offset (in local coordinates) onto a pose, modifying the pose.
    //!
    //! Adds the linear bit, then applies the rotation. 
    //!
    void addPoseOffset( float &poseX,
                        float &poseY,
                        float &poseT,
                        float offsetX,
                        float offsetY,
                        float offsetT,
                        bool  normaliseHeading );

    //!
    //! Adds an offset (in local coordinates) onto a pose, producing a result
    //!
    //! Adds the linear bit, then applies the rotation. 
    //!
    void addPoseOffset( float startX,
                        float startY,
                        float startT,
                        float offsetX,
                        float offsetY,
                        float offsetT,
                        float &resultX,
                        float &resultY,
                        float &resultT,
                        bool   normaliseHeading );

    //! normalise to [-pi,pi).
    void normaliseAngle( float &angle );

    //!
    //! We have some path, defined by an offset.
    //! This removes a chunk of it fron the front.
    //!
    void subtractInitialOffset( float &totalOffsetX,
                                float &totalOffsetY,
                                float &totalOffsetTheta,
                                float  initialOffsetX,
                                float  initialOffsetY,
                                float  initialOffsetTheta );

    //!
    //! We have some path, defined by an offset.
    //! This removes a chunk of it fron the front.
    //!
    void subtractInitialOffset( float  totalOffsetX,
                                float  totalOffsetY,
                                float  totalOffsetTheta,
                                float  initialOffsetX,
                                float  initialOffsetY,
                                float  initialOffsetTheta,
                                float &resultX,
                                float &resultY,
                                float &resultTheta );
}

#endif
