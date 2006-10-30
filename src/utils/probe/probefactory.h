/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2006 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_PROBE_FACTORY_H
#define ORCA2_PROBE_FACTORY_H

#include <orca/orca.h>
#include <orcaice/context.h>

namespace probe
{

class InterfaceProbe;
class DisplayDriver;

class ProbeFactory
{

public:

    bool isSupported( const std::string & id );

    InterfaceProbe* create( const std::string & id, const orca::FQInterfaceName & name, 
                            DisplayDriver & display, const orcaice::Context & context );

};

} // namespace

#endif
