/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#ifndef ORCAIFACEUTIL_CUSTOMDATETIME_H
#define ORCAIFACEUTIL_CUSTOMDATETIME_H

#include <string>
#include <orca/datetime.h>

namespace ifaceutil
{ 

std::string toCustomString( const orca::Time& obj, int recurse=1000, int expand=-1, int indent=0 );

}

#endif
