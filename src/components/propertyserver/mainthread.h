/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef MAIN_THREAD_H
#define MAIN_THREAD_H

#include <memory>
#include <hydroiceutil/subsystemthread.h>
#include <orcaice/context.h>
#include <orcaifaceimpl/propertiesImpl.h>
#include "propertydb.h"

namespace propserver {

//
// @brief the main executing loop of this component.
//
class MainThread : public hydroiceutil::SubsystemThread
{

public:

    MainThread( const orcaice::Context &context );

    // from SubsystemThread
    virtual void walk();

private:

    // Loops until established
    void initNetworkInterface();
    void initPropertiesDb();

    // The interface
    orcaifaceimpl::PropertiesImplPtr propertiesInterface_;

    // The database of properties
    PropertyDb propertyDb_;

    orcaice::Context context_;
};

} // namespace

#endif
