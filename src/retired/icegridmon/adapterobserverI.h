/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ADAPTEROBSERVERI_H
#define ADAPTEROBSERVERI_H

#include <string>
#include <IceGrid/Registry.h>
#include <IceGrid/Observer.h>
#include <orcaice/context.h>

namespace icegridmon
{

//
// @author Alex Brooks
//
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
