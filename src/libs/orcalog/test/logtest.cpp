/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#include <iostream>

#include <orcalog/orcalog.h>

using namespace std;

int 
main( int argc, char **argv )
{
    std::string tag;
    std::string type;
    std::string suffix;

    cout<<"testing parseRequiredTag() with empty tag ... ";
    try {
        orcalog::parseRequiredTag( tag, type, suffix );
        cout<<"failed"<<endl<<"should not be able to parse an empty tag."<<endl;
        return EXIT_FAILURE;
    }
    catch ( orcalog::Exception & )
    {
        cout<<"ok"<<endl;
    }

    cout<<"testing parseRequiredTag() with a well formed tag ... ";
    tag = "SomeTag0";
    try {
        orcalog::parseRequiredTag( tag, type, suffix );
        if ( type != "SomeTag" || suffix != "0" ) {
            cout<<"failed"<<endl<<"got type="<<type<<" sfx="<<suffix<<endl;
            exit(EXIT_FAILURE);
        }
    }
    catch ( orcalog::Exception & )
    {
        cout<<"failed"<<endl<<"should be able to parse this tag."<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;

    cout<<"testing parseRequiredTag() with a well formed tag with a large number ... ";
    tag = "SomeTag10987786745";
    try {
        orcalog::parseRequiredTag( tag, type, suffix );
        if ( type != "SomeTag" || suffix != "10987786745" ) {
            cout<<"failed"<<endl<<"got type="<<type<<" sfx="<<suffix<<endl;
            exit(EXIT_FAILURE);
        }
    }
    catch ( orcalog::Exception & )
    {
        cout<<"failed"<<endl<<"should be able to parse this tag."<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;

    cout<<"testing parseRequiredTag() with tag NOT ending in a numeral ... ";
    tag = "SomeTag";
    try {
        orcalog::parseRequiredTag( tag, type, suffix );
        cout<<"failed"<<endl<<"should not be able to parse a tag NOT ending in a numeral."<<endl;
        return EXIT_FAILURE;
    }
    catch ( orcalog::Exception & )
    {
        cout<<"ok"<<endl;
    }

    cout<<"testing parseRequiredTag() with tag which is just a numeral ... ";
    tag = "2006";
    try {
        orcalog::parseRequiredTag( tag, type, suffix );
        cout<<"failed"<<endl<<"should not be able to parse a tag which is just a numeral."<<endl;
        return EXIT_FAILURE;
    }
    catch ( orcalog::Exception & )
    {
        cout<<"ok"<<endl;
    }

    cout<<"testing headerLine() and parseHeaderLine() ... ";
    std::string filenameIn = "filenameIn";
    std::string interfaceTypeIn = "interfaceTypeIn";
    std::string formatIn = "formatIn";
    bool enabledIn = true;
    std::string headerLineOut;
    std::string filenameOut;
    std::string interfaceTypeOut;
    std::string formatOut;
    bool enabledOut;
    try {
        headerLineOut = orcalog::headerLine( filenameIn, interfaceTypeIn, formatIn, enabledIn );
        orcalog::parseHeaderLine( headerLineOut, filenameOut, interfaceTypeOut, formatOut, enabledOut );
        if ( filenameIn != filenameOut || interfaceTypeIn != interfaceTypeOut 
            || formatIn != formatOut || enabledIn != enabledOut ) {
            cout<<"failed"<<endl;
            cout<<"\tIN  : file="<<filenameIn<<" type="<<interfaceTypeIn<<" fmt="<<formatIn<<" on="<<(int)enabledIn<<endl;
            cout<<"\tOUT : string="<<headerLineOut<<endl;
            cout<<"\tOUT : file="<<filenameOut<<" type="<<interfaceTypeOut<<" fmt="<<formatOut<<" on="<<(int)enabledOut<<endl;
            exit(EXIT_FAILURE);
        }
    }
    catch ( orcalog::Exception & )
    {
        cout<<"failed"<<endl<<"should be able to write/parse this header."<<endl;
        return EXIT_FAILURE;
    }
    enabledIn = false;
    try {
        headerLineOut = orcalog::headerLine( filenameIn, interfaceTypeIn, formatIn, enabledIn );
        orcalog::parseHeaderLine( headerLineOut, filenameOut, interfaceTypeOut, formatOut, enabledOut );
        if ( filenameIn != filenameOut || interfaceTypeIn != interfaceTypeOut 
            || formatIn != formatOut || enabledIn != enabledOut ) {
            cout<<"failed"<<endl;
            cout<<"\tIN  : file="<<filenameIn<<" type="<<interfaceTypeIn<<" fmt="<<formatIn<<" on="<<(int)enabledIn<<endl;
            cout<<"\tOUT : string="<<headerLineOut<<endl;
            cout<<"\tOUT : file="<<filenameOut<<" type="<<interfaceTypeOut<<" fmt="<<formatOut<<" on="<<(int)enabledOut<<endl;
            exit(EXIT_FAILURE);
        }
    }
    catch ( orcalog::Exception & )
    {
        cout<<"failed"<<endl<<"should be able to write/parse this header."<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;

    cout<<"testing endOfHeader() and isEndOfHeader() ... ";
    try {
        if ( !orcalog::isEndOfHeader( orcalog::endOfHeader() ) ) {
            cout<<"failed"<<endl;
            exit(EXIT_FAILURE);
        }
    }
    catch ( orcalog::Exception & )
    {
        cout<<"failed"<<endl<<"should be able to write/parse end of header."<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;

    cout<<"testing parseDataLine() with empty string ... ";
    std::string dataLineIn( "" );
    int secondsOut = -1;
    int usecondsOut = -1;
    int idOut = -1;
    int indexOut = -1;
    try {
        orcalog::parseDataLine( dataLineIn, secondsOut, usecondsOut, idOut, indexOut );
        cout<<"failed"<<endl<<"should not be able to parse empy line."<<endl;
        return EXIT_FAILURE;
    }
    catch ( orcalog::Exception & )
    {
    }
    cout<<"ok"<<endl;

    cout<<"testing parseDataLine() with incomplete string ... ";
    dataLineIn = "1 2 3";    
    secondsOut = -1;
    usecondsOut = -1;
    idOut = -1;
    indexOut = -1;
    try {
        orcalog::parseDataLine( dataLineIn, secondsOut, usecondsOut, idOut, indexOut );
        cout<<"failed"<<endl<<"should not be able to parse empy line."<<endl;
        return EXIT_FAILURE;
    }
    catch ( orcalog::Exception & )
    {
    }
    cout<<"ok"<<endl;

    cout<<"testing dataLine() and parseDataLine() ... ";
    int secondsIn = 1;
    int usecondsIn = 2;
    int idIn = 3;
    int indexIn = 4;
    std::string dataLineOut;
    secondsOut = -1;
    usecondsOut = -1;
    idOut = -1;
    indexOut = -1;
    try {
        dataLineOut = orcalog::dataLine( secondsIn, usecondsIn, idIn, indexIn );
        orcalog::parseDataLine( dataLineOut, secondsOut, usecondsOut, idOut, indexOut );
        if ( secondsIn != secondsOut || usecondsIn != usecondsOut || idIn != idOut || indexIn != indexOut ) {
            cout<<"failed"<<endl;
            cout<<"\tIN  : sec="<<secondsIn<<" usec="<<usecondsIn<<" id="<<idIn<<" idx="<<indexIn<<endl;
            cout<<"\tOUT : string="<<dataLineOut<<endl;
            cout<<"\tOUT : sec="<<secondsOut<<" usec="<<usecondsOut<<" id="<<idOut<<" idx="<<indexOut<<endl;
            exit(EXIT_FAILURE);
        }
    }
    catch ( orcalog::Exception & e )
    {
        cout<<"failed"<<endl<<"should be able to write/parse this data."<<endl;
        cout<<"\texception : "<<e.what()<<endl;
        cout<<"\tOUT : line="<<dataLineOut<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;

    return EXIT_SUCCESS;
}
