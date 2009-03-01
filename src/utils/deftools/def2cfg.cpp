/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#include <iostream>
#include <fstream>

#include <orcadef/defparser.h>
#include <orcadef/exceptions.h>

using namespace std;

#define USAGE_ARGS " <.def file> <.cfg file> [--verbose]"

int main( int argc, char **argv )
{
    std::string defName = "";
    std::string cfgName = "";
    bool gotDefName=false;
    bool gotCfgName=false;
    bool verbose=false;

    for ( int i=1; i < argc; i++ )
    {
        if ( !strcmp(argv[i],"--verbose") )
        {
            verbose=true;
        }
        else if ( !gotDefName )
        {
            defName=argv[i];
            gotDefName=true;
        }
        else if ( !gotCfgName )
        {
            cfgName=argv[i];
            gotCfgName=true;
        }
        else
        {
            cout << "Usage: " << argv[0] << USAGE_ARGS << endl;
            exit(1);
        }
    }
    if ( !(gotDefName && gotCfgName) )
    {
        cout << "Usage: " << argv[0] << USAGE_ARGS << endl;
        exit(1);
    }

    orcadef::ComponentDef def;

    //
    // Parse input file
    //
    try {
        orcadef::parseDefFile(defName, def);
    }
    catch ( orcadef::FileException & e )
    {
        cerr<<e.what()<<endl;
        cerr<<"failed to open definition file. Quiting..."<<endl;
        return EXIT_FAILURE;
    }
    catch ( orcadef::ParseException & e )
    {
        cerr<<e.what()<<endl;
        cerr<<"failed to parse definition file. Quiting..."<<endl;
        return EXIT_FAILURE;
    }

    if ( verbose )
    {
        cout<< "generatecfg: parsed component def: "<<endl;
        cout<< orcadef::toString(def) << endl;
    }

    ofstream f;
    f.exceptions ( ifstream::failbit | ifstream::badbit );
    try {
        f.open( cfgName.c_str() );
    }
    catch ( fstream::failure &e )
    {
        cout<<argv[0]<<": Error opening output file '"<<cfgName<<"': "<<e.what()<<endl;
        exit(1);
    }

    //
    // Generate output file
    //
    try
    {
        f << orcadef::toCfgText( def );
        // use this one, if there's some extra stuff we want to add
        // f << orcadef::toCfgTextWithHeaders( def );
    }
    catch ( orcadef::ParseException & e )
    {
        cerr<<e.what()<<endl;
        cerr<<"failed to convert definition file to configuration. Quiting..."<<endl;
        return EXIT_FAILURE;
    }
    
    f.close();
    return 0;
}
