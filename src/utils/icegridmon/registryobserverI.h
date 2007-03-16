#ifndef REGISTRYOBSERVERI_H
#define REGISTRYOBSERVERI_H

#include <string>
#include <IceGrid/Registry.h>
#include <IceGrid/Observer.h>
#include <orcaice/context.h>

namespace icegridmon
{

//!
//! @author Alex Brooks
//!
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
