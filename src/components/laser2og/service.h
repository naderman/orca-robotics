/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef _ORCA2_LASER2OG_SERVICE_H_
#define _ORCA2_LASER2OG_SERVICE_H_

#include <orcaice/service.h>

namespace laser2og
{

//
// Class to build the component into an icebox service
//
class Service : public orcaice::Service
{

public:
    Service();

};

} // namespace

#endif
