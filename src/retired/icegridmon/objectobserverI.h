/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef OBJECTOBSERVERI_H
#define OBJECTOBSERVERI_H

#include <string>
#include <IceGrid/Registry.h>
#include <IceGrid/Observer.h>
#include <orcaice/context.h>

namespace icegridmon
{

//
// @author Alex Brooks
//
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
