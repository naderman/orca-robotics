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
#include <orcaiceutil/mathdefs.h>

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

    //! normalise to [-pi,pi).
    //  use orcaobj/mathdefs version.
    inline void normaliseAngle( double &angle )
    { NORMALISE_ANGLE(angle); }

    //!
    //! Adds an offset (in local coordinates) onto a pose, modifying the pose.
    //!
    //! Adds the linear bit, then applies the rotation. 
    //!
    inline void addPoseOffset( double &poseX,
                               double &poseY,
                               double &poseT,
                               double offsetX,
                               double offsetY,
                               double offsetT,
                               bool  normaliseHeading )
    {
        // Add x and y in the local coordinate frame
        poseX += offsetX*std::cos(poseT) - offsetY*std::sin(poseT);
        poseY += offsetX*std::sin(poseT) + offsetY*std::cos(poseT);

        // Add the heading change
        poseT = poseT + offsetT;

        if ( normaliseHeading )
        {
            // normalise to [-pi,pi).
            normaliseAngle( poseT );
        }
    }

    //!
    //! Adds an offset (in local coordinates) onto a pose, producing a result
    //!
    //! Adds the linear bit, then applies the rotation. 
    //!
    inline void addPoseOffset( double startX,
                               double startY,
                               double startT,
                               double offsetX,
                               double offsetY,
                               double offsetT,
                               double &resultX,
                               double &resultY,
                               double &resultT,
                               bool   normaliseHeading )
    {
        // Add x and y in the local coordinate frame
        resultX = startX  +  offsetX*std::cos(startT) - offsetY*std::sin(startT);
        resultY = startY  +  offsetX*std::sin(startT) + offsetY*std::cos(startT);

        // Add the heading change
        resultT = startT + offsetT;

        if ( normaliseHeading )
        {
            // normalise to [-pi,pi).
            normaliseAngle( resultT );
        }
    }

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
    inline void transformPoint2d(   double  xIn, 
                                    double  yIn,
                                    double  offsetX,
                                    double  offsetY,
                                    double  offsetT,
                                    double &xOut,
                                    double &yOut)
    {
        xOut =  std::cos(offsetT) * (xIn+offsetX) + std::sin(offsetT) * (yIn+offsetY);
        yOut = -std::sin(offsetT) * (xIn+offsetX) + std::cos(offsetT) * (yIn+offsetY);
    }  

    //! Transforms a point (x,y) in one coordinate system
    //! to a point (x,y) in another coordinate system
    //! using the offset between the two coordinate systems
    //! (translation: offsetX/Y, rotation:offsetT)
    inline void transformPoint2d( double  &x, 
                                  double  &y,
                                  double  offsetX,
                                  double  offsetY,
                                  double  offsetT )
    {
        double xNew, yNew;
        transformPoint2d( x, y, offsetX, offsetY, offsetT, xNew, yNew );
        x=xNew; y=yNew;
    }
            
}

#endif
