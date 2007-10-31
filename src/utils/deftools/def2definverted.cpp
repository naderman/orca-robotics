/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
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

#define USAGE_ARGS " <input.def file> <output.def file> [--verbose]"

int main( int argc, char **argv )
{
    std::string defInName = "";
    std::string defOutName = "";
    bool gotDefInName=false;
    bool gotDefOutName=false;
    bool verbose=false;

    for ( int i=1; i < argc; i++ )
    {
        if ( !strcmp(argv[i],"--verbose") )
        {
            verbose=true;
        }
        else if ( !gotDefInName )
        {
            defInName=argv[i];
            gotDefInName=true;
        }
        else if ( !gotDefOutName )
        {
            defOutName=argv[i];
            gotDefOutName=true;
        }
        else
        {
            cout << "Usage: " << argv[0] << USAGE_ARGS << endl;
            exit(1);
        }
    }
    if ( !(gotDefInName && gotDefOutName) )
    {
        cout << "Usage: " << argv[0] << USAGE_ARGS << endl;
        exit(1);
    }

    orcadef::ComponentDef def;

    //
    // Parse input file
    //
    try {
        orcadef::parseDefFile(defInName, def);
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
        cout<< "invertdef: parsed component def: "<<endl;
        cout<< orcadef::toString(def) << endl;
    }

    ofstream f;
    f.open( defOutName.c_str() );
    if ( !f.is_open() )
    {
        cout<<argv[0]<<": Error opening ouput file '"<<defOutName<<"'"<<endl;
        exit(1);
    }

    //
    // Invert the input file
    //
    orcadef::ComponentDef invDef;
    invDef = orcadef::invertDef( def, "Mate" );

    //
    // Generate output file
    //
    try
    {
        f << orcadef::toDefText( invDef );
    }
    catch ( orcadef::ParseException & e )
    {
        cerr<<e.what()<<endl;
        cerr<<"failed to invert definition file. Quiting..."<<endl;
        return EXIT_FAILURE;
    }
    
    f.close();
    return 0;
}
