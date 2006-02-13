/*
 *  Orca Project: Components for robotics.
 *
 *  Copyright (C) 2004-2006
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef ORCA2_TELEOP_USER_HANDLER_H
#define ORCA2_TELEOP_USER_HANDLER_H

#include <orcaice/thread.h>
#include <orcaice/context.h>
#include <orcaice/ptrbuffer.h>

#include <orca/platform2d.h>

#include "inputdriver.h"


class UserHandler : public orcaice::Thread
{
public:

    UserHandler( orcaice::PtrBuffer<orca::Velocity2dCommandPtr> *commands );
    virtual ~UserHandler();

    void setCurrent( const orcaice::Context & context ) { context_=context; };

    virtual void run();

private:

    // network/driver interface
    orcaice::PtrBuffer<orca::Velocity2dCommandPtr> *commandBuffer_;

    // generic interface to input hardware
    InputDriver* driver_;

    InputDriver::Config config_;

    InputDriver::DriverType driverType_;

    void readConfigs();

    // component current context
    orcaice::Context context_;
};

#endif
