#ifndef OBJECTOBSERVERI_H
#define OBJECTOBSERVERI_H

#include <string>
#include <IceGrid/Registry.h>
#include <IceGrid/Observer.h>
#include <orcaice/context.h>

namespace icegridmon
{

//!
//! @author Alex Brooks
//!
class ObjectObserverI : public IceGrid::ObjectObserver
{

public: 

    ObjectObserverI( const orcaice::Context &context )
        : context_(context) {}

    void objectInit( const IceGrid::ObjectInfoSeq &objects,
                      const Ice::Current& );

    void objectAdded( const IceGrid::ObjectInfo &info,
                       const Ice::Current& );

    void objectRemoved( const Ice::Identity &id,
                        const Ice::Current& );

    void objectUpdated( const IceGrid::ObjectInfo &info,
                         const Ice::Current& );                             

private: 

    const orcaice::Context context_;

};

}

#endif
