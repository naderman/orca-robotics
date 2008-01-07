/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include "events.h"

using namespace teleop;

bool 
TeleopEventQueueOptimizer::combine( hydroiceutil::EventPtr& existing, const hydroiceutil::EventPtr& extra ) 
{ 
    if ( existing->type()==IncrementCommand && extra->type()==IncrementCommand ) 
    {    
        IncrementCommandEventPtr eExisting = IncrementCommandEventPtr::dynamicCast( existing );
        IncrementCommandEventPtr eExtra = IncrementCommandEventPtr::dynamicCast( extra );
        if ( !eExisting || !eExtra ) {
            return false; 
        }
        // incremental commands add up
        eExisting->longitudinal    += eExtra->longitudinal;
        eExisting->transverse      += eExtra->transverse;
        eExisting->angular         += eExtra->angular;
        return true;
    }
    else if ( existing->type()==IncrementCommand && extra->type()==RelativeCommand ) 
    {    
        IncrementCommandEventPtr eExisting = IncrementCommandEventPtr::dynamicCast( existing );
        RelativeCommandEventPtr eExtra = RelativeCommandEventPtr::dynamicCast( extra );
        if ( !eExisting || !eExtra ) {
            return false; 
        }
        // relative command simply replaces any incremental one
        existing = extra;
        return true;
    }
    else if ( existing->type()==RelativeCommand && extra->type()==RelativeCommand ) 
    {    
        RelativeCommandEventPtr eExisting = RelativeCommandEventPtr::dynamicCast( existing );
        RelativeCommandEventPtr eExtra = RelativeCommandEventPtr::dynamicCast( extra );
        if ( !eExisting || !eExtra ) {
            return false; 
        }
        // newer relative command replaces an older one
        existing = extra;
        return true;
    }
    // an incremental command following a relative one cannot be combined
    return false;
};
