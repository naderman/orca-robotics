/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef _ORCA2_OGNODE_SERVICE_H_
#define _ORCA2_OGNODE_SERVICE_H_

#include <orcaice/service.h>

namespace ognode
{

//
// Class to build the component into an icebox service
//
class Service : public orcaice::Service
{

public:
    Service();

};

} // ognode

#endif
