/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2007-current Alex Brooks, Alex Makarenko
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <colourtext.h>
#include <iostream>
#include <vector>
#include <sstream>

using namespace std;
using namespace hydroctext;

int main()
{
    std::vector<Attribute> attrs;
    std::vector<string> attrNames;

    attrs.push_back( Bold );
    attrs.push_back( Dim );
    attrs.push_back( Underline );
    attrs.push_back( Blink );
    attrs.push_back( Reverse );
    attrs.push_back( NormalAttributes );

    attrNames.push_back( "Bold            ");
    attrNames.push_back( "Dim             ");
    attrNames.push_back( "Underline       ");
    attrNames.push_back( "Blink           ");
    attrNames.push_back( "Reverse         ");
    attrNames.push_back( "NormalAttributes");

    std::vector<Colour> colours;
    std::vector<string> colourNames;

    colours.push_back( Black        );
    colours.push_back( Red          );
    colours.push_back( Green        );
    colours.push_back( Yellow       );
    colours.push_back( Blue         );
    colours.push_back( Magenta      );
    colours.push_back( Cyan         );
    colours.push_back( White        );
    colours.push_back( DefaultColour );

    colourNames.push_back( "Black        " );
    colourNames.push_back( "Red          " );
    colourNames.push_back( "Green        " );
    colourNames.push_back( "Yellow       " );
    colourNames.push_back( "Blue         " );
    colourNames.push_back( "Magenta      " );
    colourNames.push_back( "Cyan         " );
    colourNames.push_back( "White        " );
    colourNames.push_back( "DefaultColour" );

    stringstream ss;

    for ( size_t i=0; i <attrs.size(); i++ ) {
        for ( size_t j=0; j < colours.size(); j++ ) {
            for ( size_t k=0; k < colours.size(); k++ ) {
                ss << set( attrs[i], colours[j], colours[k] )
                   << " attr: " << attrNames[i]
                   << " fg: " << colourNames[j]
                   << " bg: " << colourNames[k]
                   << reset() << endl;
            }
        }
    }

    ss << set( NormalAttributes, White ) << "asdfasdf" << reset() << " fdjksfds" << endl;

//     ss << set( NormalAttributes, Green );
//     ss <<"TRACE(colourtest.cpp): blah" << endl;

//     ss << set( Bold, Red );
//     ss <<"TRACE(colourtest.cpp): blah" << endl;

//     ss << set( NormalAttributes, Yellow );
//     ss <<"TRACE(colourtest.cpp): blah" << endl;

//     ss << set( NormalAttributes, Blue );
//     ss <<"TRACE(colourtest.cpp): blah" << endl;

//     ss << set( NormalAttributes, Magenta );
//     ss <<"TRACE(colourtest.cpp): blah" << endl;

//     ss << set( NormalAttributes, Cyan );
//     ss <<"TRACE(colourtest.cpp): blah" << endl;

//     ss <<"TRACE(colourtest.cpp): before reset" << endl;
//     ss << reset();
//     ss <<"TRACE(colourtest.cpp): after reset" << endl;
    
    cout << ss.str();

    return 0;
}
