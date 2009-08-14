/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <sstream>
#include "humaninput2d.h"

namespace hydrointerfaces {

void 
HumanInput2d::Command::resetIncremental()
{
    isLongIncrement = true;
    isTransverseIncrement = true;
    isAngularIncrement = true;

    longitudinal = 0.0;
    transverse = 0.0;
    angular = 0.0;
};

void 
HumanInput2d::Command::resetAbsolute()
{
    isLongIncrement = false;
    isTransverseIncrement = false;
    isAngularIncrement = false;

    longitudinal = 0.0;
    transverse = 0.0;
    angular = 0.0;
};

std::string 
HumanInput2d::Command::toString() const
{
    std::stringstream ss;
    
    if ( isLongIncrement ) 
        ss<<"deltaLong=";
    else
        ss<<"setLong=";
    ss<<longitudinal<<" ";
    
    if ( isTransverseIncrement ) 
        ss<<"deltaTrans=";
    else
        ss<<"setTrans=";
    ss<<transverse<<" ";
    
    if ( isAngularIncrement )
        ss<<"deltaAng=";
    else
        ss<<"setAng=";
    ss<<angular<<std::endl;
    
    return ss.str();
}

} // namespace
