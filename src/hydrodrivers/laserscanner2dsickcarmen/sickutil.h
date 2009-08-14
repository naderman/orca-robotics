/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef SICK_LASER_UTIL_H
#define SICK_LASER_UTIL_H

namespace sickutil {

bool isConfigValid( double rangeResolution, double angleIncrement, int baudrate=38400 );

double rangeResolution2MaxRange( double rangeResolution );

double maxRange2RangeResolution( double maxRange );

double numberOfSamples2AngleIncrement( int numberOfSamples );

int angleIncrement2NumberOfSamples( double angleIncrement );

}

#endif
