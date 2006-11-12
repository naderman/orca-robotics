/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef SICK_LASER_UTIL_H
#define SICK_LASER_UTIL_H

namespace sicklaserutil {

bool  sickCanDoConfig( double rangeResolution, double angleIncrement,
                       double maxRange, int baudrate=38400 );

float associatedMaxRange( double rangeResolution );

}

#endif
