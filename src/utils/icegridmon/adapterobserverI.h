#ifndef ADAPTEROBSERVERI_H
#define ADAPTEROBSERVERI_H

#include <string>
#include <IceGrid/Registry.h>
#include <IceGrid/Observer.h>
#include <orcaice/context.h>

namespace icegridmon
{

//!
//! @author Alex Brooks
//!
class AdapterObserverI : public IceGrid::AdapterObserver
{

public: 

    AdapterObserverI( const orcaice::Context &context )
        : context_(context) {}

    void adapterInit( const IceGrid::AdapterInfoSeq &adpts,
                      const Ice::Current& );

    void adapterAdded( const IceGrid::AdapterInfo &info,
                       const Ice::Current& );

    void adapterRemoved( const std::string &id,
                         const Ice::Current& );

    void adapterUpdated( const IceGrid::AdapterInfo &info,
                         const Ice::Current& );                             

private: 

    const orcaice::Context context_;

};

}

#endif
