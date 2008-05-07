/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef REGISTRYOBSERVERI_H
#define REGISTRYOBSERVERI_H

#include <string>
#include <IceGrid/Registry.h>
#include <IceGrid/Observer.h>
#include <orcaice/context.h>

namespace icegridmon
{

//
// @author Alex Brooks
//
class RegistryObserverI : public IceGrid::RegistryObserver
{

public: 

    RegistryObserverI( const orcaice::Context &context )
        : context_(context) {}

    void registryInit( const IceGrid::RegistryInfoSeq &registries,
                       const Ice::Current& );

    void registryUp( const IceGrid::RegistryInfo &info,
                     const Ice::Current& );

    void registryDown( const std::string &name,
                       const Ice::Current& );

private: 

    const orcaice::Context context_;

};

}

#endif
