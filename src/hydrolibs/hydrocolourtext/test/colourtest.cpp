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
#include <unistd.h>
#include <assert.h>

using namespace std;
using namespace hydroctext;

Attribute stringToAttribute( const std::string& s )
{
    Attribute attr;
    if ( s=="Bold" )
        attr = Bold;
    else if ( s=="Dim" )
        attr = Dim;
    else if ( s=="Underline" )
        attr = Underline;
    else if ( s=="Blink" )
        attr = Blink;
    else if ( s=="Reverse" )
        attr = Reverse;
    else if ( s=="NormalAttributes" )
        attr = NormalAttributes;
    else {
        cout << "Could not parse attribute name: "<<s<<endl;
        assert( false );
    }
    return attr;
};

Colour stringToColour( const std::string& s )
{
    Colour col;
    if ( s=="Black" )
        col = Black;
    else if ( s=="Red" )
        col = Red;
    else if ( s=="Green" )
        col = Green;
    else if ( s=="Yellow" )
        col = Yellow;
    else if ( s=="Blue" )
        col = Blue;
    else if ( s=="Magenta" )
        col = Magenta;
    else if ( s=="Cyan" )
        col = Cyan;
    else if ( s=="White" )
        col = White;
    else if ( s=="DefaultColour" )
        col = DefaultColour;
    else  {
        cout << "Could not parse colour name: "<<s<<endl;
        assert( false );
    }
    return col;
};

void setAllOptions( vector<Attribute>& attrs )
{
    attrs.push_back( Bold );
    attrs.push_back( Dim );
    attrs.push_back( Underline );
    attrs.push_back( Blink );
    attrs.push_back( Reverse );
    attrs.push_back( NormalAttributes );
}

void setAllOptions( vector<Colour>& colours )
{
    colours.push_back( Black        );
    colours.push_back( Red          );
    colours.push_back( Green        );
    colours.push_back( Yellow       );
    colours.push_back( Blue         );
    colours.push_back( Magenta      );
    colours.push_back( Cyan         );
    colours.push_back( White        );
    colours.push_back( DefaultColour );
}

string toString( Attribute attr )
{
    string name;
    switch ( attr )
    {
        case Bold :
            name = "Bold            ";
            break;
        case Dim :
            name = "Dim             ";
            break;
        case Underline :
            name = "Underline       ";
            break;
        case Blink :
            name = "Blink           ";
            break;
        case Reverse :
            name = "Reverse         ";
            break;
        case NormalAttributes :
            name = "NormalAttributes";
            break;
    }
    return name;
}

string toString( Colour colour )
{
    string name;
    switch ( colour )
    {
        case Black :
            name = "Black        ";
            break;
        case Red :
            name = "Red          ";
            break;
        case Green :
            name = "Green        ";
            break;
        case Blue :
            name = "Blue         ";
            break;
        case Yellow :
            name = "Yellow       ";
            break;
        case Magenta :
            name = "Magenta      ";
            break;
        case Cyan :
            name = "Cyan         ";
            break;
        case White :
            name = "White        ";
            break;
        case DefaultColour :
            name = "DefaultColour";
            break;
    }
    return name;
}

int main( int argc, char **argv )
{
    std::vector<Attribute> attrs;
    std::vector<Colour> fgColours;
    std::vector<Colour> bgColours;

    int opt;

    // Get some options from the command line
    while ((opt = getopt(argc, argv, "a:f:b:")) != -1)
    {
        switch ( opt )
        {
        case 'a':
            attrs.push_back( stringToAttribute( string(optarg) ) );
            break;
        case 'f':
            fgColours.push_back( stringToColour( string(optarg) ) );
            break;
        case 'b':
            bgColours.push_back( stringToColour( string(optarg) ) );
            break;
        default:
            cout << "Usage: " << argv[0] << " [-a attribute] [-f foreground] [-b background]" << endl << endl
                 << "-a attribute   One of {Bold,Dim,Underline,Blink,Reverse,NormalAttributes}" << endl
                 << "-f foreground  One of {Black,Red,Green,Yellow,Blue,Magenta,Cyan,White,DefaultColour}" << endl
                 << "-b background  One of the colours above" << endl << endl
                 << "The unspecified options default to a vector of all possible values, resulting in a" << endl
                 << "{Nattr x Ncol x Ncol} combinations" << endl;
            return 1;
        }
    }

    if ( attrs.empty() )
        setAllOptions( attrs );

    if ( fgColours.empty() )
        setAllOptions( fgColours );

    if ( bgColours.empty() )
        setAllOptions( bgColours );

    cout<<"rendering "<<attrs.size()*fgColours.size()*bgColours.size()<<" combinations: "
          "[A x F x B]=["<<attrs.size()<<"x"<<fgColours.size()<<"x"<<bgColours.size()<<"]"<<endl;

    stringstream ss;

    for ( size_t i=0; i <attrs.size(); i++ ) {
        for ( size_t j=0; j < fgColours.size(); j++ ) {
            for ( size_t k=0; k < bgColours.size(); k++ ) {
                ss << set( attrs[i], fgColours[j], bgColours[k] )
                   << " attr: " << toString(attrs[i])
                   << " fg: " << toString(fgColours[j])
                   << " bg: " << toString(bgColours[k])
                   << reset() << endl;
            }
        }
    }

//     ss << set( NormalAttributes, White ) << "asdfasdf" << reset() << " fdjksfds" << endl;

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
