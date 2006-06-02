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

#include <iostream>
#include <orcaice/orcaice.h>

#include "position2dconsumerI.h"

using namespace std;

Position2dConsumerI::Position2dConsumerI( orcaice::PtrBuffer<orca::Position2dDataPtr> & pipe )
    : pipe_(pipe)
{
}

void Position2dConsumerI::setData(const orca::Position2dDataPtr& data, const Ice::Current&)
{
//     cout<<"pub : "<<data<<endl;
    pipe_.push( data );
}
