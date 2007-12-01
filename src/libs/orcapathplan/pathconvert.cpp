/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include "pathconvert.h"
#include <orcaice/orcaice.h>
#include <sstream>

using namespace std;

namespace orcapathplan {

std::string toString( const WaypointParameter &wp )
{
    stringstream ss;
    ss << "[dTol="<<wp.distanceTolerance<<", headTol="<<wp.headingTolerance*180.0/M_PI<<"deg, time="<<wp.timeTarget.seconds<<":"<<wp.timeTarget.useconds<<", maxAppSpeed="<<wp.maxApproachSpeed<<"m/s, maxAppTurn="<<wp.maxApproachTurnrate*180.0/M_PI<<"deg/s]";
    return ss.str();
}

void setWpParameters( const WaypointParameter &para, orca::Waypoint2d &wp )
{
    wp.distanceTolerance = para.distanceTolerance;
    wp.headingTolerance = para.headingTolerance;
    wp.timeTarget = para.timeTarget;
    wp.maxApproachSpeed = para.maxApproachSpeed;
    wp.maxApproachTurnrate = para.maxApproachTurnrate;
}

void setHeading( orca::PathPlanner2dData &pathData, double firstHeading)
{
    for (unsigned int i=0; i<pathData.path.size(); i++ )
    {
        if (i==0) 
        {
            pathData.path[i].target.o = firstHeading;
        } 
        else 
        {
            float diffX =  pathData.path[i].target.p.x - pathData.path[i-1].target.p.x;
            float diffY =  pathData.path[i].target.p.y - pathData.path[i-1].target.p.y;
            float heading = atan2(diffY,diffX);
            pathData.path[i].target.o = heading;
            // correct past headings to the direction we're going
            if (i>1)
            {
                pathData.path[i-1].target.o = heading;
            }
        } 
    }
}

// void 
// convert( const hydropathplan::Result  result,
//          orca::PathPlanner2dData     &output )
// {
//     switch(result)
//     {
//         case hydropathplan::PathOk: 
//             output.result = orca::PathOk;
//             break;
//         case hydropathplan::PathStartNotValid: 
//             output.result = orca::PathStartNotValid;
//             break;
//         case hydropathplan::PathDestinationNotValid: 
//             output.result = orca::PathDestinationNotValid;
//             break;
//         case hydropathplan::PathDestinationUnreachable: 
//             output.result = orca::PathDestinationUnreachable;
//             break;
//         case hydropathplan::OtherError:
//             output.result = orca::PathOtherError;
//             break;   
//         default:
//             output.result = orca::PathOtherError;
//             break;
//     }

// }
    
bool 
isDoubleWaypoint( const orca::PathPlanner2dData &path, float worldX, float worldY )
{
    int sizePath = path.path.size();
    if (sizePath!=0)    
    {
        if( NEAR(path.path[sizePath-1].target.p.x, worldX, 0.01) &&
            NEAR(path.path[sizePath-1].target.p.y, worldY, 0.01) )
        {
            std::cout << "TRACE(pathconvert.cpp): We have a double waypoint" << std::endl;
            return true;           
        }
    }
    return false;
}

void 
convertAndAppend( const hydroogmap::OgMap           &ogMap,
                  const hydropathplan::Cell2DVector &input,
                  orca::PathPlanner2dData           &output,
                  double                             firstHeading)
{
    double worldX, worldY;
    orca::Waypoint2d wp;
    orcaice::setInit( wp );

    for( unsigned int i=0; i<input.size(); i++ )
    {
        ogMap.getWorldCoords( input[i].x(), input[i].y(), worldX, worldY );
        
        if ( ( i==0 ) && ( isDoubleWaypoint(output,worldX,worldY)) ) continue;
        
        wp.target.p.x = worldX;
        wp.target.p.y = worldY;
        output.path.push_back( wp );
    }
    
    setHeading( output, firstHeading );
}
        

void 
convertAndAppend( const hydroogmap::OgMap              &ogMap,
                  const hydropathplan::Cell2DVector    &input,
                  const std::vector<WaypointParameter> &wpPara,
                  orca::PathPlanner2dData              &output,
                  double                                firstHeading)
{
    
    double worldX, worldY;
    orca::Waypoint2d wp;
    orcaice::setInit( wp );

    for( unsigned int i=0; i<input.size(); i++ )
    {
        
        ogMap.getWorldCoords( input[i].x(), input[i].y(), worldX, worldY );
        
        if ( ( i==0 ) && ( isDoubleWaypoint(output,worldX,worldY)) ) continue;
        
        wp.target.p.x = worldX;
        wp.target.p.y = worldY;
        setWpParameters( wpPara[i], wp );
        output.path.push_back( wp );
    }
    
    setHeading( output, firstHeading );
}


void setParameters( orca::PathPlanner2dData &pathData,
                    const std::vector<WaypointParameter> &wpPara )
{
    assert( pathData.path.size() == wpPara.size() );
    for (unsigned int i=0; i<pathData.path.size(); i++)
    {
        orca::Waypoint2d &wp = pathData.path[i];
        setWpParameters( wpPara[i], wp );
    }
}

}

