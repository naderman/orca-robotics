/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include "pathconvert.h"
#include <orcaice/orcaice.h>

using namespace std; // for math functions

namespace orcapathplan {

    void convert( const orcapathplan::Result        result,
                  const orca::PathPlanner2dDataPtr &output )
    {
        switch(result)
        {
            case PathOk: 
                output->result = orca::PathOk;
                break;
            case PathStartNotValid: 
                output->result = orca::PathStartNotValid;
                break;
            case PathDestinationNotValid: 
                output->result = orca::PathDestinationNotValid;
                break;
            case PathDestinationUnreachable: 
                output->result = orca::PathDestinationUnreachable;
                break;
            case OtherError:
                output->result = orca::OtherError;
                break;   
            default:
                output->result = orca::OtherError;
                break;
        }

    }
    
    bool isDoubleWaypoint( const orca::PathPlanner2dDataPtr &path, float worldX, float worldY )
    {
        int sizePath = path->path.size();
        if (sizePath!=0)    
        {
            if( NEAR(path->path[sizePath-1].target.p.x, worldX, 0.01) &&
                NEAR(path->path[sizePath-1].target.p.y, worldY, 0.01) )
            {
                std::cout << "TRACE(pathconvert.cpp): We have a double waypoint" << std::endl;
                return true;           
            }
        }
        return false;
    }
    
    void convert( const orcaogmap::OgMap               &ogMap,
                  const Cell2DVector                   &input,
                  const orcapathplan::Result            result,
                  const orca::PathPlanner2dDataPtr     &output )
    {
        convert( result, output );

        float worldX, worldY;
        orca::Waypoint2d wp;
        orcaice::setInit( wp );

        for( unsigned int i=0; i<input.size(); i++ )
        {
            ogMap.getWorldCoords( input[i].x(), input[i].y(), worldX, worldY );
            
            if ( ( i==0 ) && ( isDoubleWaypoint(output,worldX,worldY)) ) continue;
            
            wp.target.p.x = worldX;
            wp.target.p.y = worldY;
            output->path.push_back( wp );
        }
    }
    
    void convert( const orcaogmap::OgMap               &ogMap,
                  const Cell2DVector                   &input,
                  const std::vector<WaypointParameter> &wpPara,
                  const orcapathplan::Result            result,
                  const orca::PathPlanner2dDataPtr     &output )
    {
        convert( result, output );
        
        float worldX, worldY;
        orca::Waypoint2d wp;
        orcaice::setInit( wp );

        for( unsigned int i=0; i<input.size(); i++ )
        {
            
            ogMap.getWorldCoords( input[i].x(), input[i].y(), worldX, worldY );
            
            if ( ( i==0 ) && ( isDoubleWaypoint(output,worldX,worldY)) ) continue;
            
            wp.target.p.x = worldX;
            wp.target.p.y = worldY;

            wp.distanceTolerance = wpPara[i].distanceTolerance;
            wp.headingTolerance = wpPara[i].headingTolerance;
            wp.timeTarget = wpPara[i].timeTarget;
            wp.maxApproachSpeed = wpPara[i].maxApproachSpeed;
            wp.maxApproachTurnrate = wpPara[i].maxApproachTurnrate;
            output->path.push_back( wp );
        }
        
        // add headings, by default they're aligned with the connection between waypoints
        for (unsigned int i=0; i<output->path.size(); i++ )
        {
            if (i==0) 
            {
                output->path[i].target.o = 0.0;
            } 
            else 
            {
                float diffX =  output->path[i].target.p.x -output->path[i-1].target.p.x;
                float diffY =  output->path[i].target.p.y -output->path[i-1].target.p.y;
                float heading = atan2(diffY,diffX);
                output->path[i].target.o = heading;
                // correct past headings to the direction we're going
                if (i>1)
                {
                    output->path[i-1].target.o = heading;
                }
            } 
        }
            
    }

}

