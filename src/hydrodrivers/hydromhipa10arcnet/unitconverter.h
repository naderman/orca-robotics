/*************************************************************************
	
	date		: August 2008
	copyright	: (C) 2005 J.D. Yamokoski
	email	: yamokosk+gnu at gmail dot com

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public License as
 published by the Free Software Foundation; either version 2.1 of the License,
 or (at your option) any later version. The text of the GNU Lesser General
 Public License is included with this library in the file LICENSE.TXT.
 
 This library is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the file LICENSE.TXT for
 more details.
 
 *************************************************************************/

#ifndef HYDRO_MHIPA10_UNITCONVERTER_H
#define HYDRO_MHIPA10_UNITCONVERTER_H

#include <stdint.h>

namespace mhipa10arc
{

//
// @brief convert between S.I. units and MHI counts
//
// @author J.D. Yamokoski
//

// Counts -> S.I. units
//! Convert counts to radians
double angleFromCounts( int32_t counts );
//! Convert counts to radians/s
float speedFromCounts( int16_t counts );
//! Convert counts to N*m
float torqueFromCounts( unsigned int servoNum, int16_t counts );

// S.I. units -> counts
//! Convert torque (N*m) to counts
int16_t torqueToCounts(unsigned int servoNum, float value);
//! Convert speed (rad/s) to counts
int16_t speedToCounts(float value);


}

#endif
