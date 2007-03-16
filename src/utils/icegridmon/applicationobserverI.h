#ifndef APPLICATIONOBSERVERI_H
#define APPLICATIONOBSERVERI_H

#include <string>
#include <IceGrid/Registry.h>
#include <IceGrid/Observer.h>
#include <orcaice/context.h>

namespace icegridmon
{

//!
//! @author Alex Brooks
//!
class ApplicationObserverI : public IceGrid::ApplicationObserver
{

public: 

    ApplicationObserverI( const orcaice::Context &context )
        : context_(context) {}

    void applicationInit( int serial,
                          const IceGrid::ApplicationInfoSeq &applications,
                          const Ice::Current& );

    void applicationAdded( int serial, 
                           const IceGrid::ApplicationInfo &desc,
                           const Ice::Current& );

    void applicationRemoved( int serial, 
                             const std::string &name,
                             const Ice::Current& );

    void applicationUpdated( int serial,
                             const IceGrid::ApplicationUpdateInfo &desc,
                             const Ice::Current& );                             

private: 

    const orcaice::Context context_;

};

}

#endif
