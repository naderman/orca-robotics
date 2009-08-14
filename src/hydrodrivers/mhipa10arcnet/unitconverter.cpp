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

#include <cmath>
#include <hydrointerfaces/mhipa10.h>

#include "unitconverter.h"
#include "mhidefs.h"


namespace mhipa10arc {

double angleFromCounts( int32_t counts )
{
	return ((double)counts) * COUNT_TO_DEGREE * M_PI/180.0;
}

float speedFromCounts( int16_t counts )
{
	return ((float)counts) * COUNT_TO_SPEED;
}

float torqueFromCounts( unsigned int servoNum, int16_t counts )
{
	return ((float)counts) * COUNT_TO_TORQUE * hydrointerfaces::MitsubishiPA10::maxRatedMotorTorque[servoNum];
}

int16_t speedToCounts(float value)
{
	return (int16_t)roundf(value * SPEED_TO_COUNT);
}

int16_t torqueToCounts(unsigned int servoNum, float value)
{
	return (int16_t)roundf(TORQUE_TO_COUNT * value/hydrointerfaces::MitsubishiPA10::maxRatedMotorTorque[servoNum]);
}

}
