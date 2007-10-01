/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
 
#include <iostream>

#include <orcadef/orcadef.h>

using namespace std;

int 
main( int argc, char **argv )
{
    if ( argc != 2 ) {
        cerr<<"Expecting 1 command line argument: input file name"<<endl;
        return EXIT_FAILURE;
    }

    orcadef::ComponentDef def;

    cout<<"testing parseDefFile() ... ";
    // try to open a non-existant file
    try {
        orcadef::parseDefFile( "does_not_exist.def", def );
        cout<<"failed"<<endl<<"file 'does_not_exist.def' should not exist"<<endl;
        return EXIT_FAILURE;
    }
    catch ( orcadef::FileException & )
    {
        // ok
    }

    std::string inputFileName(argv[1]);
    try {
        orcadef::parseDefFile( inputFileName, def );
    }
    catch ( orcadef::FileException & )
    {
        cout<<"failed"<<endl<<"input file should exist"<<endl;
        return EXIT_FAILURE;
    }
    catch ( orcadef::ParseException & )
    {
        cout<<"failed"<<endl<<"input file should be parsable"<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;

    orcadef::ComponentDef empty;
    orcadef::ComponentCfg cfg;
    cout<<"testing toCfg() ... ";
    // try to parse an empty structure
    try {
        cfg = orcadef::toCfg( empty );
        cout<<"failed"<<endl<<"def structure should be non-parsable"<<endl;
        return EXIT_FAILURE;
    }
    catch ( orcadef::ParseException & )
    {
        // ok
    }

    try {
        cfg = orcadef::toCfg( def );
    }
    catch ( orcadef::ParseException & )
    {
        cout<<"failed"<<endl<<"def structure should be parsable"<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;

    // check that component properties are as expected
    for ( unsigned int i=0; i<def.configs.size(); ++i ) {
        if ( def.configs[i].tag == "Max" ) {
            if ( def.configs[i].value != "default value" ) {
                cout<<"failed"<<endl<<"default value parsed incorrectly for 'Ext' :"<<def.configs[i].value<<endl;
                return EXIT_FAILURE;
            }
        }
        //
        // This syntax is OBSOLETE, use ExternalConfig tag
        //
        if ( def.configs[i].tag == "External.One.Two" ) {
            if ( def.configs[i].isExternal == false ) {
                cout<<"failed"<<endl<<"external part of config parsed incorrectly for 'External.One.Two'"<<endl;
                return EXIT_FAILURE;
            }
        }
        if ( def.configs[i].tag == "Max" || def.configs[i].tag == "Min" ) {
            if ( def.configs[i].isExternal == true ) {
                cout<<"failed"<<endl<<"external part of config parsed incorrectly for 'Max' or 'Min''"<<endl;
                return EXIT_FAILURE;
            }
        }
    }

    // check that external properties are as expected
    for ( unsigned int i=0; i<def.externalConfigs.size(); ++i ) {
        if ( def.externalConfigs[i].tag == "Avg" ) {
            if ( def.externalConfigs[i].value != "99" ) {
                cout<<"failed"<<endl<<"default value parsed incorrectly for 'Ext' :"<<def.externalConfigs[i].value<<endl;
                return EXIT_FAILURE;
            }
        }
    }

    // alexm: what else can we test?
    
    // debug
    cout<<"*** DEF ***"<<endl<<toString( def )<<endl;
    cout<<"*** CFG ***"<<endl<<toString( cfg )<<endl;
    cout<<"*** CFG TEXT ***"<<endl<<toCfgText( def )<<endl;
//     cout<<"*** CFG TEXT WITH HEADERS ***"<<endl<<toCfgTextWithHeaders( def )<<endl;
    cout<<"*** XML INSTANCE ***"<<endl<<toXmlText( def )<<endl;
    cout<<"*** XML TEMPLATE ***"<<endl<<toXmlTemplateText( def )<<endl;

    return EXIT_SUCCESS;
}
