/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef NODEOBSERVERI_H
#define NODEOBSERVERI_H

#include <string>
#include <IceGrid/Registry.h>
#include <IceGrid/Observer.h>
#include <orcaice/context.h>

namespace icegridmon
{

//
// @author Alex Brooks
//
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
