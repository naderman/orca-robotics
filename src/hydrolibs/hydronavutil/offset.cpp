/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#include <hydronavutil/hydronavutil.h>
#include <iostream>
#include <sstream>
#include <cmath>

// M_PI is not defined after including cmath for the MS visual studio compiler?
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using namespace std; // for math functions in QNX

namespace hydronavutil {

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
