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

#ifndef TELEOP_EVDEVUTIL_H
#define TELEOP_EVDEVUTIL_H

namespace teleop
{

// Functions to help debug things.
void printInputType( const char *dev );
void printDeviceInfo( const char *dev );

int devSupportsAbsoluteAxes( const char *dev, bool &supportsAbsoluteAxes );
int devIsUSB( const char *dev, bool &isUSB );

// returns 0 on success
// Caller should have allocated at least nameSize to name.
int devName( const char *dev, char *name, int nameSize=256 );

} // namespace

#endif
