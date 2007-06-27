/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2007-current Alex Brooks, Alex Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include "colourtext.h"
#include <iostream>
#include <sstream>

using namespace std;

//
// Simple text colours on the console.
// 
// See: man console_codes
//

namespace orcactext {

std::string reset()
{
    const int RESET = 0;

    char command[13];
    /* Command is the control command to the terminal */
    sprintf(command, "%c[%dm", 0x1B, RESET);
    stringstream ss;
    ss << command;
    return ss.str();
}

std::string set( Attribute attr, Colour fg, Colour bg )
{
    char command[13];
    /* Command is the control command to the terminal */
    sprintf(command, "%c[%d;%d;%dm", 0x1B, attr, fg + 30, bg + 40);
    stringstream ss;
    ss << command;
    return ss.str();
}
    
std::string set( const Style& style )
{
    return set( style.attr, style.fg, style.bg );
}

std::string emph( const std::string& s, const Style& emph )
{
    return set(emph) + s + reset();
}

std::string emph( const std::string& s, const Style& emph, const Style& main )
{
    return set(emph) + s + set(main);
}

} // namespace
