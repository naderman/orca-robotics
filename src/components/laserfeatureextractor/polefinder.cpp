/*
 *  Orca-Components: Components for robotics.
 *  
 *  Copyright (C) 2004
 *  
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include "polefinder.h"
#include <math.h>
#include <iostream>
#include <assert.h>

using namespace std;

using namespace orca_polefinder;
using namespace orca;

int 
orca_polefinder::detect_poles( const LaserConfigPtr laserConf,
                               const LaserDataPtr ranges,
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
    int    min_clearance_from_dodge_in_returns = (int) (rint( min_angle_from_dodge / laserConf->angleIncrement ));

    for ( int i = 0; i < ranges->ranges.size(); i++ )
    {
        if ( (i != 0) )
        {
            delta_range = ranges->ranges[i] - ranges->ranges[previous_point];

            if ( potential_pole_start )
            {
                // We could be looking at a pole here...
                pole_width = ranges->ranges[potential_pole_start]*sin( (i-potential_pole_start)*laserConf->angleIncrement );

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
                        temp_range   = ranges->ranges[(potential_pole_start+(i-1))/2];
                        temp_bearing = (((i-1)+potential_pole_start)/2)*laserConf->angleIncrement + ranges->startAngle;
                        
                        poles.resize( poles.size()+1 );
                        poles[poles.size()-1].range   = temp_range;
                        poles[poles.size()-1].bearing = temp_bearing;

                        // cout<<"TRACE(polefinder.cpp): Adding pole at " << temp_range << ", " << temp_bearing << endl;
                        // cout<<"TRACE(polefinder.cpp): num Points: " << i-potential_pole_start+1 << endl;
                        
                        num_poles++;
                        potential_pole_start = 0;
                        last_pole_end_in_returns = i;
                        continue;
                    }
                    else
                    {
                        // Pole was wrong width.
                        // cout<<"TRACE(polefinder.cpp): bad width at ("<<ranges.ranges((potential_pole_start+i)/2)<<","<<((i+potential_pole_start)/2)*ranges.angleIncrement() + ranges.bearingStart()*180/M_PI<<"): " << pole_width << endl;
                        
                        potential_pole_start = 0;
                    }
                }
                else if (fabs(ranges->ranges[i] - ranges->ranges[potential_pole_start]) > max_width)
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
            if ( ranges->ranges[i] == max_laser_range || fabs(delta_range) > min_distance_to_background )
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
