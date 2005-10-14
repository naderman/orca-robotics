/*
 *  Orca-Base: Components for robotics.
 *
 *  Copyright (C) 2004
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

#include <pthread.h>

#include "platform2d_i.h"

#include <orca/objutils.h>

Platform2dI::Platform2dI()
{
    position_ = new orca::Position2dData;
    orcaiceutil::setSane( position_ );
}

orca::Position2dDataPtr Platform2dI::getData(const Ice::Current& current)
{
    std::cout << "Sending data back" << std::endl;
    return position_;
}
