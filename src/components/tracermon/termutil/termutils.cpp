/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <orcaice/orcaice.h>

#include "termutils.h"

using namespace std;

std::string
tracermon::toString( const orca::TracerData& obj )
{
    string s = "[ ";

    s += orcaice::toString( obj.timeStamp ) + " ";

    s += orcaice::toString( obj.name );
    // todo: would be nice to show level here, e.g. debug3:
    // but seems like an overkill to deal with osstreams
    s += " " + obj.category + ": ";
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
