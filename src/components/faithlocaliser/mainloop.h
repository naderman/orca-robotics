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
 
#ifndef ORCA2_FAITH_LOCALISER_MAIN_LOOP_H
#define ORCA2_FAITH_LOCALISER_MAIN_LOOP_H

#include <orcaice/thread.h>
#include <orcaice/ptrbuffer.h>
#include <orcaice/context.h>
#include <orca/localise2d.h>

namespace faithlocaliser
{

// Note: this thing self-destructs when run() returns.
class MainLoop : public orcaice::Thread
{

public:

    MainLoop( const orca::Localise2dConsumerPrx                localise2dConsumer,
              orcaice::PtrBuffer<orca::Position2dDataPtr>     &posBuffer,
 	      orcaice::PtrBuffer<orca::Localise2dDataPtr>     &locBuffer,
	      orcaice::PtrBuffer<orca::Localise2dDataPtr>     &historyBuffer,
	      double                                           stdDevPosition,
              double                                           stdDevHeading,
	      orcaice::Context                                 context
	    );
    ~MainLoop();

    virtual void run();

private:

    // IceStorm consumer
    const orca::Localise2dConsumerPrx  localise2dConsumer_;

    // incoming
    orcaice::PtrBuffer<orca::Position2dDataPtr> &posBuffer_;
    
    // outgoing
    orcaice::PtrBuffer<orca::Localise2dDataPtr> &locBuffer_;
    orcaice::PtrBuffer<orca::Localise2dDataPtr> &historyBuffer_;

    orcaice::Context context_;

    double stdDevPosition_;

    double stdDevHeading_;
};

} // namespace

#endif
