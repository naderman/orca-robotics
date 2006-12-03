/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2006 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ORCAPROBEFACTORY_HOME_INTERFACE_PROBE_H
#define ORCA2_ORCAPROBEFACTORY_HOME_INTERFACE_PROBE_H

#include <orcaprobe/interfaceprobe.h>

namespace orcaprobefactory
{

class HomeProbe : public orcaprobe::InterfaceProbe
{

public:

    HomeProbe( const orca::FQInterfaceName & name, orcaprobe::DisplayDriver & display,
                                const orcaice::Context & context );

    virtual int loadOperation( const int index );
    
private:
        
    int loadGetInterfaces();
    int loadGetProperties();

};

} // namespace

#endif
