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

#ifndef ORCA2_PLATFORM2D_INTERFACE_ICE
#define ORCA2_PLATFORM2D_INTERFACE_ICE

#include <orca/position2d.ice>
#include <orca/velocity2ddata.ice>

module orca
{

//! Platform2d is an "active" Position2d. In addition to serving its current position2d
//! it also accepts motion commands (should be in the form of velocity not position).
interface Platform2d extends Position2d
{
        // ClientPush_Consumer interface
        void putData( Velocity2dData data );
};

}; // module

#endif
