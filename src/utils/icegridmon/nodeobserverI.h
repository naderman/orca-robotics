#ifndef NODEOBSERVERI_H
#define NODEOBSERVERI_H

#include <string>
#include <IceGrid/Registry.h>
#include <IceGrid/Observer.h>
#include <orcaice/context.h>

namespace icegridmon
{

//!
//! @author Alex Brooks
//!
class NodeObserverI : public IceGrid::NodeObserver
{

public: 

    NodeObserverI( const orcaice::Context &context )
        : context_(context) {}

    void nodeInit( const IceGrid::NodeDynamicInfoSeq &nodes,
                   const Ice::Current& );

    void nodeUp( const IceGrid::NodeDynamicInfo &info,
                 const Ice::Current& );

    void nodeDown( const std::string &name,
                   const Ice::Current& );

    void updateServer( const std::string &node, 
                       const IceGrid::ServerDynamicInfo &updatedInfo,
                       const Ice::Current& );

                       void updateAdapter( const std::string &node,
                                           const IceGrid::AdapterDynamicInfo &updatedInfo,
                                           const Ice::Current& );

    void nodeUpdated( const IceGrid::NodeInfo &info,
                      const Ice::Current& );                             

private: 

    const orcaice::Context context_;

};

}

#endif
