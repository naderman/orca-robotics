/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef MAIN_THREAD_H
#define MAIN_THREAD_H

#include <memory>
#include <orcaice/subsystemthread.h>
#include <orcaice/context.h>
#include <orcaifaceimpl/properties.h>
#include <hydropropertydb/propertydb.h>

namespace propserver {

//
// @brief the main executing loop of this component.
//
class MainThread : public orcaice::SubsystemThread
{

public:

    MainThread( const orcaice::Context &context );

private:
    // from SubsystemThread
    virtual void initialise();
    virtual void work();

    // Loops until established
    void initNetworkInterface();
    void initPropertiesDb();

    // The interface
    orcaifaceimpl::PropertiesImplPtr propertiesInterface_;

    // The database of properties
    hydropropertydb::PropertyDb propertyDb_;

    // Where we load/save our properties
    std::string persistanceFile_;

    orcaice::Context context_;
};

} // namespace

#endif
