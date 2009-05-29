/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include "tracer.h"
#include "datetime.h"
#include "ocm.h"
#include <orcaifaceutil/tracer.h>

using namespace std;

namespace orcaobj
{

std::string toString( const orca::TracerData& obj )
{
    std::string s;

    s = "[ ";
    s += toString( obj.timeStamp ) + " ";
    s += toString( obj.name );
    s += " " + ifaceutil::toString(obj.type) + ": ";
    s += obj.message + " ]";

    // replace line breaks with spaces
    string::size_type idx = 0;
    while((idx = s.find("\n", idx)) != string::npos)
    {
        s.insert(idx + 1, "  ");
        ++idx;
    }

    return s;
}

} // namespace
