/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef HYDRONAVUTIL_OFFSET__H
#define HYDRONAVUTIL_OFFSET__H

#include <iostream>
#include <gbxutilacfr/mathdefs.h>

//
// A collection of small utility functions for doing
// navigation-related things.
//
// @author Alex Brooks, Tobias Kaupp
//

namespace hydronavutil {

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
    //! Opposite of addPoseOffset.  Subtracts an offset from the end.
    //!
    inline void subtractFinalPoseOffset( double startX,
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
        double st = std::sin(startT-offsetT);
        double ct = std::cos(startT-offsetT);
        resultX = startX  -  offsetX*ct + offsetY*st;
        resultY = startY  -  offsetX*st - offsetY*ct;

        resultT = startT-offsetT;

        if ( normaliseHeading )
            normaliseAngle( resultT );
    }

    //!
    //! Opposite of addPoseOffset.  Subtracts an offset from the end.
    //!
    inline void subtractFinalPoseOffset( double &poseX,
                                         double &poseY,
                                         double &poseT,
                                         double offsetX,
                                         double offsetY,
                                         double offsetT,
                                         bool  normaliseHeading )
    {
        double resX, resY, resT;
        subtractFinalPoseOffset( poseX, poseY, poseT,
                                 offsetX, offsetY, offsetT,
                                 resX, resY, resT,
                                 normaliseHeading );
        poseX = resX; poseY = resY; poseT = resT;
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

    //! Convenience function for 'double*'s
    inline void addPoseOffset( double *pose, const double *offset, bool normaliseHeading )
    { 
        addPoseOffset( pose[0], pose[1], pose[2],
                       offset[0], offset[1], offset[2],
                       normaliseHeading ); 
    }

    //! Convenience function for 'double*'s
    inline void addPoseOffset( const double *start, const double *offset, double *result, bool normaliseHeading )
    {
        addPoseOffset( start[0], start[1], start[2],
                       offset[0], offset[1], offset[2],
                       result[0], result[1], result[2],
                       normaliseHeading );
    }

    //! Convenience function for 'double*'s
    inline void subtractFinalPoseOffset( double *pose, const double *offset, bool normaliseHeading )
    { 
        subtractFinalPoseOffset( pose[0], pose[1], pose[2],
                                 offset[0], offset[1], offset[2],
                                 normaliseHeading ); 
    }

    //! Convenience function for 'double*'s
    inline void subtractFinalPoseOffset( const double *start, const double *offset, double *result, bool normaliseHeading )
    {
        subtractFinalPoseOffset( start[0], start[1], start[2],
                                 offset[0], offset[1], offset[2],
                                 result[0], result[1], result[2],
                                 normaliseHeading );
    }

    inline void subtractInitialOffset( double *totalOffset,
                                       const double *initialOffset )
    {
        return subtractInitialOffset( totalOffset[0], totalOffset[1], totalOffset[2],
                                      initialOffset[0], initialOffset[1], initialOffset[2] );
    }

    inline void subtractInitialOffset( const double *totalOffset,
                                       const double *initialOffset,
                                       double *result )
    {
        return subtractInitialOffset( totalOffset[0], totalOffset[1], totalOffset[2],
                                      initialOffset[0], initialOffset[1], initialOffset[2],
                                      result[0], result[1], result[2] );
    }

    inline void getInverseTransform( double tX, double tY, double tTheta,
                                     double &invTX, double &invTY, double &invTTheta )
    {
        double st = sin(-tTheta);
        double ct = cos(-tTheta);

        invTX = -( ct*tX - st*tY );
        invTY = -( st*tX + ct*tY );
        invTTheta = -tTheta;
    }
}

#endif
