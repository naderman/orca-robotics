/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef SERVICE_I_H
#define SERVICE_I_H

#include <orcaice/service.h>

namespace featuremaploader {
//
// Class to build the component into an icebox service
//
class ServiceI : public orcaice::Service
{

public:
    ServiceI();

};

}

#endif
