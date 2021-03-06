/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_HUMANINPUT2D_JOYSTICK_EVDEVUTIL_H
#define ORCA2_HUMANINPUT2D_JOYSTICK_EVDEVUTIL_H

#include <string>

namespace humaninput2djoystick
{

// Throws std::string with error when something goes wrong.
void findUSBJoystick( std::string& joystickDevice );

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
