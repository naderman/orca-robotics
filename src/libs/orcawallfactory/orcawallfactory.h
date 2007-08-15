/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2007 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_DEFAULT_WALL_FACTORY_H
#define ORCA2_DEFAULT_WALL_FACTORY_H

#include <orcawall/factory.h>

namespace orcawallfactory
{

class OrcaWallFactory : public orcawall::Factory
{

public:
    OrcaWallFactory();

    virtual orcawall::InterfaceSim* create(
                                  const std::string& interfaceType,
                                  const std::string& tag,
                                  const orcaice::Context& context );
};

} // namespace

extern "C" {
    orcawall::Factory *createFactory();
}

#endif
