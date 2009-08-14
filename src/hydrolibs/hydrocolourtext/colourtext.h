/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2007-current Alex Brooks, Alex Makarenko
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef HYDRO_COLOUR_TEXT_H
#define HYDRO_COLOUR_TEXT_H

#include <string>

namespace hydroctext {

////////////////////////////////////////

//! Text attribute
enum Attribute {
    //! Bold
    Bold             = 1,
    //! Dim
    Dim              = 2,
    //! Underline
    Underline        = 4,
    //! Blink
    Blink            = 5,
    //! Reverse
    Reverse          = 7,
    //! Normal
    NormalAttributes = 22
};
    
//! Text colour
enum Colour {
    //! Black
    Black         = 0,
    //! Red
    Red           = 1,
    //! Green
    Green         = 2,
    //! Yellow
    Yellow        = 3,
    //! Blue
    Blue          = 4,
    //! Magenta
    Magenta       = 5,
    //! Cyan
    Cyan          = 6,
    //! White
    White         = 7,
    //! Default
    DefaultColour = 9
};

//! Combines attribute and colour into one structure
struct Style
{
    //! Sets all fields to default values
    Style( Attribute a=NormalAttributes, Colour f=DefaultColour, Colour b=DefaultColour ) :
        attr(a),
        fg(f),
        bg(b) {};
    //! Attribute
    Attribute attr;
    //! Foreground (text) colour
    Colour fg;
    //! Background colour
    Colour bg;
};

////////////////////////////////////////

//! Returns a string with the escape-codes required to return the display to default values.
std::string reset();

//! Returns a string with escape codes which will change the style of terminal output.
//! This style will persist until another change is made or the style is reset.
//! @see emph()
std::string set( Attribute attr=NormalAttributes, Colour fg=DefaultColour, Colour bg=DefaultColour );

//! Convenience function, behaves the same as the above.
std::string set( const Style& style );

//! Adds escape codes before and after the supplied string so that it is displayed using 
//! Style @e emph but the following text is displayed with default style (this is achieved by calling
//! reset()).
//! @see set()
std::string emph( const std::string& s, const Style& emph );

//! Adds escape codes before and after the supplied string so that it is displayed using 
//! Style @e emph but the following text is displayed with Style @e main.
//! @see set()
std::string emph( const std::string& s, const Style& emph, const Style& main );

//! Truncates or pads the string to the specified length
//! The string may contain escape codes: they will not be counted
std::string toFixedWidth( const std::string &s, int length );

} // namespace

#endif
