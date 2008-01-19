/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_ORCAWALLFACTORY_ODOMETRY2D_INTERFACE_SIM_H
#define ORCA_ORCAWALLFACTORY_ODOMETRY2D_INTERFACE_SIM_H

#include <orcawall/interfacesim.h>

namespace orcawallfactory
{

class Odometry2dSim : public orcawall::InterfaceSim
{

public:
    Odometry2dSim( const std::string& tag, const orcaice::Context& context );

    // from hydroiceutil::Thread
    virtual void walk();
};

} // namespace

#endif
