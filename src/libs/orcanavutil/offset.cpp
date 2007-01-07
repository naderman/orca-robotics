/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#include "orcanavutil.h"
#include <iostream>
#include <sstream>
#include <cmath>

// M_PI is not defined after including cmath for the MS visual studio compiler?
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using namespace std; // for math functions in QNX

namespace orcanavutil {

    std::ostream &operator<<( std::ostream &s, const Offset &o )
    {
        s << "["  << o.x 
          << ", " << o.y 
          << ", " << o.theta*180.0/M_PI 
          << "deg]";
        return s;
    }

    std::string toString( const Offset &o )
    {
        std::stringstream ss;
        ss << o;
        return ss.str();
    }


    void addPoseOffset( double &poseX,
                        double &poseY,
                        double &poseT,
                        double  offsetX,
                        double  offsetY,
                        double  offsetT,
                        bool   normaliseHeading )
    {
        // Add x and y in the local coordinate frame
        poseX += offsetX*cos(poseT) - offsetY*sin(poseT);
        poseY += offsetX*sin(poseT) + offsetY*cos(poseT);

        // Add the heading change
        poseT = poseT + offsetT;

        if ( normaliseHeading )
        {
            // normalise to [-pi,pi).
            normaliseAngle( poseT );
        }
    }

    void addPoseOffset( double  startX,
                        double  startY,
                        double  startT,
                        double  offsetX,
                        double  offsetY,
                        double  offsetT,
                        double &resultX,
                        double &resultY,
                        double &resultT,
                        bool   normaliseHeading )
    {
        // Add x and y in the local coordinate frame
        resultX = startX  +  offsetX*cos(startT) - offsetY*sin(startT);
        resultY = startY  +  offsetX*sin(startT) + offsetY*cos(startT);

        // Add the heading change
        resultT = startT + offsetT;

        if ( normaliseHeading )
        {
            // normalise to [-pi,pi).
            normaliseAngle( resultT );
        }
    }

    void normaliseAngle( double &angle )
    {
        // Normalise to [-pi,pi)
        while ( angle >= M_PI )
            angle -= 2*M_PI;
        while ( angle < -M_PI )
            angle += 2*M_PI;
    }

    void subtractInitialOffset( double &totalOffsetX,
                                double &totalOffsetY,
                                double &totalOffsetTheta,
                                double  initialOffsetX,
                                double  initialOffsetY,
                                double  initialOffsetTheta )
    {
        // First, remove the linear part by translating
        double remainingOffsetX = totalOffsetX - initialOffsetX;
        double remainingOffsetY = totalOffsetY - initialOffsetY;

        // Then, rotate the remaining offset by -initialOffsetTheta
        totalOffsetX =   remainingOffsetX*cos(-initialOffsetTheta) 
                       - remainingOffsetY*sin(-initialOffsetTheta);
        totalOffsetY =   remainingOffsetX*sin(-initialOffsetTheta) 
                       + remainingOffsetY*cos(-initialOffsetTheta);

        totalOffsetTheta -= initialOffsetTheta;
    }


    void subtractInitialOffset( double  totalOffsetX,
                                double  totalOffsetY,
                                double  totalOffsetTheta,
                                double  initialOffsetX,
                                double  initialOffsetY,
                                double  initialOffsetTheta,
                                double &resultX,
                                double &resultY,
                                double &resultTheta )
    {
        // First, remove the linear part by translating
        double remainingOffsetX = totalOffsetX - initialOffsetX;
        double remainingOffsetY = totalOffsetY - initialOffsetY;

        // Then, rotate the remaining offset by -initialOffsetTheta
        resultX =   remainingOffsetX*cos(-initialOffsetTheta) 
                       - remainingOffsetY*sin(-initialOffsetTheta);
        resultY =   remainingOffsetX*sin(-initialOffsetTheta) 
                       + remainingOffsetY*cos(-initialOffsetTheta);

        resultTheta = totalOffsetTheta - initialOffsetTheta;
    }
}
