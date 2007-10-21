/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include "events.h"

using namespace teleop;

bool 
TeleopEventQueueOptimizer::combine( hydroutil::EventPtr& existing, const hydroutil::EventPtr& extra ) 
{ 
    if ( existing->type()==NewCommandIncrement && extra->type()==NewCommandIncrement ) 
    {    
        NewCommandIncrementEventPtr eExisting = NewCommandIncrementEventPtr::dynamicCast( existing );
        NewCommandIncrementEventPtr eExtra = NewCommandIncrementEventPtr::dynamicCast( extra );
        if ( !eExisting || !eExtra ) {
            return false; 
        }
        // incremental commands add up
        eExisting->longitudinal_    += eExtra->longitudinal_;
        eExisting->transverse_      += eExtra->transverse_;
        eExisting->angle_           += eExtra->angle_;
        return true;
    }
    else if ( existing->type()==NewCommandIncrement && extra->type()==NewRelativeCommand ) 
    {    
        NewCommandIncrementEventPtr eExisting = NewCommandIncrementEventPtr::dynamicCast( existing );
        NewRelativeCommandEventPtr eExtra = NewRelativeCommandEventPtr::dynamicCast( extra );
        if ( !eExisting || !eExtra ) {
            return false; 
        }
        // relative command simply replaces any incremental one
        existing = extra;
        return true;
    }
    else if ( existing->type()==NewRelativeCommand && extra->type()==NewRelativeCommand ) 
    {    
        NewRelativeCommandEventPtr eExisting = NewRelativeCommandEventPtr::dynamicCast( existing );
        NewRelativeCommandEventPtr eExtra = NewRelativeCommandEventPtr::dynamicCast( extra );
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
