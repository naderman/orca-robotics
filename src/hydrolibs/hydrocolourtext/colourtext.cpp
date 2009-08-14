/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2007-current Alex Brooks, Alex Makarenko
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include "colourtext.h"
#include <iostream>
#include <sstream>
#include <stdio.h>

using namespace std;

//
// Simple text colours on the console.
// 
// See: man console_codes
//

namespace hydroctext {

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

std::string toFixedWidth( const std::string &s, int w )
{
        
    const int ESCAPE = 0x1B;
                
    std::string padded;
    int numNonEscape=0;
    bool parsingEscape=false;
    for ( unsigned int i=0; i < s.size(); i++ )
    {
        if ( s[i] == ESCAPE )
            parsingEscape = true;
                
        if ( parsingEscape )
        {
            padded += s[i];
        }
        else if ( numNonEscape < w )
        {
            padded += s[i];
            numNonEscape++;
        }
                
        if ( s[i] == 'm' && parsingEscape )
        {
            parsingEscape = false;
        }
    }
                
    for ( int i=numNonEscape; i < w; i++ )
        padded += " ";
                
    return padded;
}

} // namespace
