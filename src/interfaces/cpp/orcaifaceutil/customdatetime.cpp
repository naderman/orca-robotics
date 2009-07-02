/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include "customdatetime.h"
#include <IceUtil/Time.h>

namespace ifaceutil
{ 

std::string toCustomString( const orca::Time& obj, int recurse, int expand, int indent ) 
{ 
    IceUtil::Time t = IceUtil::Time::seconds(obj.seconds) + IceUtil::Time::milliSeconds(obj.useconds); 
    return t.toDateTime();
}

}
