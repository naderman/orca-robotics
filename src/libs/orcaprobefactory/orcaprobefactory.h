/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2007 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_DEFAULT_PROBE_FACTORY_H
#define ORCA2_DEFAULT_PROBE_FACTORY_H

#include <orcaprobe/factory.h>

namespace orcaprobefactory
{

class OrcaProbeFactory : public orcaprobe::Factory
{

public:
    OrcaProbeFactory();

    virtual orcaprobe::InterfaceProbe* create( 
                const std::string           & interfaceType, 
                const orca::FQInterfaceName & name, 
                orcaprobe::IDisplay    & display, 
                const orcaice::Context      & context );

};

} // namespace

extern "C" {
    orcaprobe::Factory *createFactory();
}

#endif
