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
 
#ifndef ORCA2_STAGE_LOCALISER_COMPONENT_H
#define ORCA2_STAGE_LOCALISER_COMPONENT_H

#include <orcaice/component.h>
#include <orcaice/ptrbuffer.h>

#include <orca/localise2d.h>

namespace simlocaliser
{

class MainLoop;

// Deriving from orcaice::Component means we have to implement start() and stop()
class Component : public orcaice::Component
{
public:

    Component();
    virtual ~Component();

    virtual void start();
    virtual void stop();

private:

    //
    // EXTERNAL PROVIDED INTERFACE
    //
    orcaice::PtrBuffer<orca::Localise2dDataPtr> locBuffer_;

    MainLoop *mainLoop_;
};

} // namespace

#endif
