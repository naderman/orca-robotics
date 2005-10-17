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

#include "platform2d_i.h"
#include <orcaiceutil/objutils.h>

using namespace std;
using namespace orca;
using namespace orcaiceutil;

Platform2dI::Platform2dI( orcaiceutil::PtrBuffer* position2d, orcaiceutil::PtrBuffer* command ) :
                    position2d_(position2d), command_(command)
{
}

// served out the data to the client (it was stored here by the driver at the last read)
orca::Position2dDataPtr Platform2dI::getData(const Ice::Current& current) const
{
    std::cout << "Sending data back" << std::endl;

    // create a null pointer. data will be cloned into it.
    Ice::ObjectPtr data;
    //! @todo what should happens if there's no data?
    position2d_->peek( data );

    cout<<Position2dDataPtr::dynamicCast( data )<<endl;
    return Position2dDataPtr::dynamicCast( data );
}

orca::Position2dGeometryPtr Platform2dI::getGeometry(const Ice::Current& current) const
{
    std::cout << "Pretending to send geometry back" << std::endl;
    Position2dGeometryPtr geometry = new Position2dGeometry;

    return geometry;
}

// Store incoming command in a proxy, it will be handled by the driver at the next opportunity.
void Platform2dI::putData(const ::orca::Velocity2dCommandPtr& command, const ::Ice::Current& )
{
    command_->push( command );
}
