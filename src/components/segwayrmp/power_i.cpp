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

#include "power_i.h"

using namespace orca;


PowerI::PowerI( orcaiceutil::PtrBuffer<orca::PowerDataPtr>* power ) :
        powerProxy_(power)
{
}

orca::PowerDataPtr PowerI::getData(const ::Ice::Current& ) const
{
    std::cout << "Sending data back" << std::endl;

    // create null smart pointer. data will be cloned into it.
    orca::PowerDataPtr data;

    // we don't need to pop the data here because we don't block on it.
    // we always want to have the latest copy in there
    //! @todo what happens if there's no data?    
    powerProxy_->get( data );

    return data;
}
