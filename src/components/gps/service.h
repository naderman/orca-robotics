/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Mathew Ridley, Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_GPS_SERVICE_H
#define ORCA2_GPS_SERVICE_H

#include <orcaice/service.h>

namespace gps
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
