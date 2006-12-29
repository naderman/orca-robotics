/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCAICE_LOCAL_HOME_H
#define ORCAICE_LOCAL_HOME_H

#include "../home.h"

namespace orcaice
{
namespace detail
{

class LocalHome : public Home
{
public:
    virtual ~LocalHome() {}; 
    
    virtual void addProvidedInterface( const orca::ProvidedInterface& iface ){};

    virtual void addRequiredInterface( const orca::RequiredInterface& iface ){};

    virtual void removeProvidedInterface( const std::string& name ) {};

    virtual void removeRequiredInterface( const std::string& name ) {};
};

} // namespace
} // namespace

#endif
