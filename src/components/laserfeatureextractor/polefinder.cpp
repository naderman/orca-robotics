/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp,
 *               George Mathews, Stef Williams.
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <math.h>
#include <iostream>
#include <assert.h>

#include "polefinder.h"

using namespace std;
using namespace orca_polefinder;

int 
orca_polefinder::detect_poles( const std::vector<float> &scan,
                               double startAngle,
                               double angleIncrement,
                               double max_laser_range,
                               double min_width,
                               double max_width,
                               double min_distance_to_background,
                               double min_angle_from_dodge,
                               std::vector<positionRB> &poles )
{
    poles.clear();

    double delta_range = 0;
    int    potential_pole_start = 0;
    double pole_width;
    int    num_poles = 0;
    int    previous_point = 0;
    double temp_range;
    float  temp_bearing;
    int    last_dodgy_range = 0;
    int    last_pole_end_in_returns = -99999999;
    int    min_clearance_from_dodge_in_returns = (int) (rint( min_angle_from_dodge / angleIncrement ));

    for ( int i = 0; i < (int)scan.size(); i++ )
    {
        if ( (i != 0) )
        {
            delta_range = scan[i] - scan[previous_point];

            if ( potential_pole_start )
            {
                // We could be looking at a pole here...
                pole_width = scan[potential_pole_start]*sin( (i-potential_pole_start)*angleIncrement );

                if ( pole_width > max_width )
                {
                    // This is too big to be a pole
                    potential_pole_start = 0;
                }
                if ( delta_range > max_width*2.0/3.0 )
                {
                    // Cluster ends.  Is it a pole?

                    // Check for end of pole
                    // The last condition says we shouldn't declare a pole off just a single point.
                    if ( (pole_width > min_width) && 
                         (pole_width < max_width) &&
                         (i-potential_pole_start > 1) )
                    {
                        // We have a pole!
                        temp_range   = scan[(potential_pole_start+(i-1))/2];
                        temp_bearing = (((i-1)+potential_pole_start)/2)*angleIncrement + startAngle;
                        
                        poles.resize( poles.size()+1 );
                        poles[poles.size()-1].range   = temp_range;
                        poles[poles.size()-1].bearing = temp_bearing;

//                         cout<<"TRACE(polefinder.cpp): Adding pole at " 
//                             << temp_range << ", " << temp_bearing*180.0/M_PI << "deg" << endl;
//                         cout<<"TRACE(polefinder.cpp): num Points: " << i-potential_pole_start+1 << endl;
                        
                        num_poles++;
                        potential_pole_start = 0;
                        last_pole_end_in_returns = i;
                        continue;
                    }
                    else
                    {
                        // Pole was wrong width.
                        // cout<<"TRACE(polefinder.cpp): bad width at ("<<scan((potential_pole_start+i)/2)<<","<<((i+potential_pole_start)/2)*ranges.angleIncrement() + ranges.bearingStart()*180/M_PI<<"): " << pole_width << endl;
                        
                        potential_pole_start = 0;
                    }
                }
                else if (fabs(scan[i] - scan[potential_pole_start]) > max_width)
                {
                    // All points on a 'pole' have to be within
                    // a certain range of one another
                    potential_pole_start = 0;
                }
            }

            // Start thinking about a new pole if the range steps forward and we're far enough from the last dodgy bit.
            if ( (-delta_range) > min_distance_to_background && 
                 (i-last_dodgy_range) > min_clearance_from_dodge_in_returns &&
                 (i-last_pole_end_in_returns) > min_clearance_from_dodge_in_returns )
            {
                potential_pole_start = i;
            }

            // Is there anything screwy about this return?
            if ( scan[i] >= max_laser_range ||
                 fabs(delta_range) > min_distance_to_background )
            {
                // Yes, this range is screwy.
                if ( !potential_pole_start )
                {
                    last_dodgy_range = i;
                }

                // Is there dodgy shit right after the pole (note that the first return after the pole is 
                // gonna be dodgy, coz it's a big step after the pole!)
                if ( (i-last_pole_end_in_returns < min_clearance_from_dodge_in_returns) &&
                     (i-last_pole_end_in_returns > 1 ) )
                {
//                     cout<<"TRACE(polefinder.cpp): i: " << i << endl;
//                     cout<<"TRACE(polefinder.cpp): last_pole_end_in_returns: " << last_pole_end_in_returns << endl;
//                     cout<<"TRACE(polefinder.cpp): i-last_pole_end_in_returns: " << i-last_pole_end_in_returns << endl;
//                     cout<<"TRACE(polefinder.cpp): min_clearance_from_dodge_in_returns: " << min_clearance_from_dodge_in_returns << endl;
//                     cout<<"TRACE(polefinder.cpp): potential_pole_start : " << potential_pole_start << endl;

                    if ( poles.size() > 0 )
                    {
                        // Invalidate the last pole we found.
//                        cout<<"TRACE(polefinder.cpp): WARNING: invalidating previous pole at angle " << 180/M_PI*(poles[poles.size()-1].bearing)<< " due to subsequent dodginess" << endl;
                        poles.resize( poles.size()-1 );
                        num_poles--;
                    }
                }
            }

            
            previous_point = i;
        }
    }

    return num_poles;
}
