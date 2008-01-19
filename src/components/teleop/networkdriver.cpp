/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include "networkdriver.h"

#include <hydroutil/mathdefs.h>

using namespace teleop;

void 
NetworkDriver::incrementValue( double& value, double delta, double minValue, double maxValue )
{    
    if ( delta ) {
        value += delta;
        value = MIN( value, maxValue );
        value = MAX( value, minValue );
    }
}

void 
NetworkDriver::setValue( double& value, double newValue, double minValue, double maxValue )
{    
    if ( value != newValue ) {
        value = newValue;
        value = MIN( value, maxValue );
        value = MAX( value, minValue );
    }
}
