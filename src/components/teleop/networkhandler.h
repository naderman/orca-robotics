/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_TELEOP_NETWORK_HANDLER_H
#define ORCA2_TELEOP_NETWORK_HANDLER_H

#include <orcaice/thread.h>
#include <orcaice/context.h>
#include <orcaice/ptrbuffer.h>

#include <orca/platform2d.h>

namespace teleop
{

class DisplayHandler;

class NetworkHandler : public orcaice::Thread
{
public:

    NetworkHandler( orcaice::Buffer<orca::Velocity2dCommand> *commandBuffer,
                    DisplayHandler* displayHandler, const orcaice::Context & context );
    ~NetworkHandler();

    void setupConfigs( const Ice::PropertiesPtr & properties );

    virtual void run();

private:

    // network/driver interface
    orcaice::Buffer<orca::Velocity2dCommand>* commandBuffer_;

    DisplayHandler* displayHandler_;

    // component current context
    orcaice::Context context_;

};

} // namespace

#endif
