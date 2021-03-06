/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <string>

#include "iostreamdisplaydriver.h"

using namespace std;
using namespace orcateleop;

int 
IostreamDisplayDriver::enable()
{
    return 0;
}

int 
IostreamDisplayDriver::disable()
{
    return 0;
}

void 
IostreamDisplayDriver::show( const std::string& s )
{
    cout << s << endl;
}

