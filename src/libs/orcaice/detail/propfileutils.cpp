/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
    
#include "propfileutils.h"
#include <orcaice/proputils.h>
#include <gbxutilacfr/exceptions.h>

using namespace std;

namespace orcaice
{
namespace detail
{

std::string getGlobalConfigFilename( const Ice::PropertiesPtr &props )
{
    std::string filename;

    // first look for Orca.GlobalConfig parameter
    int ret = orcaice::getProperty( props, "Orca.GlobalConfig", filename );
    if ( ret == 0 )
    {
        return filename;
    }

    // now look in this special environment variable
    char *orca_config = getenv("ORCA_GLOBAL_CONFIG");
    if ( orca_config != NULL ) {
        // debug
        cout<<"echo $ORCA_GLOBAL_CONFIG='"<<orca_config<<"'"<<endl;
        // this extra test is mostly because Win32 does not implement
        // unsetenv(). So we have to set the env var to empty string instead.
        if ( strlen(orca_config) ) {
            filename = orca_config;
            return filename;
        }
    }

    // nothing is specified, assume default filename and directory
#ifndef WIN32
    // Linux and friends
    char *home = getenv("HOME");
    if ( home == NULL ) {
        throw gbxutilacfr::Exception( ERROR_INFO, "Environment variable 'HOME' not set, while trying to load .orcarc" );
    }
    
    // start with the home directory
    filename = home;
    // assume the name of the global config file
    filename += "/.orcarc";
#else
    // windows
    filename += "C:\\orca.ini";
#endif
    return filename;
}

std::string 
getApplicationConfigFilename( const std::vector<std::string> & args )
{
    if ( args.empty() ) {
        return std::string( "" );
        // throwing this exception leads to 'unhandled ex' in windows for some reason
        //throw gbxutilacfr::Exception( ERROR_INFO, "Invalid command line options, cannot determine config file name" );
    }

    // first look for Orca.Config parameter
    for ( unsigned int i=0; i<args.size(); ++i ) {
        if ( !args[i].compare( 0,13, "--Orca.Config" ) ) {
            return args[i].substr( 14, args[i].size()-1 );
        }
    }

    // now look for Ice.Config parameter (specified by IceGrid)
    for ( unsigned int i=0; i<args.size(); ++i ) {
        if ( !args[i].compare( 0,12, "--Ice.Config" ) ) {
            return args[i].substr( 13, args[i].size()-1 );
        }
    }

    // if there's only one argument (other than the program name), 
    // assume it's the path to the config file
    // ... unless it starts with a '-', i.e. looks like an option
    if ( args.size()==2 && args[1].substr(0,1)!="-" ) {
//         cout<<"single argument starts with :"<<args[1].substr(0,1)<<endl;
        return args[1];
    }

    // if there are no arguments (other than the program name),
    // assume a) the config file is in the current directory
    //        b) config file name = executable + '.cfg'
    if ( args.size()==1 ) {
//         cout<<"no arguments"<<endl;
        // strip the path from the executable
#ifndef WIN32
        int lastSlash = args[0].rfind( '/', args[0].size()-1 );
#else
        int lastSlash = args[0].rfind( '\\', args[0].size()-1 );
#endif
        string exeName;
        if ( lastSlash < 0 ) {
            // slash was not found in the executable path
            // not sure why size()-1 clips the last character of the exe name
            exeName = args[0].substr( lastSlash+1, args[0].size() );
        }
        else {
            exeName = args[0].substr( lastSlash+1, args[0].size()-1 );
        }
#ifdef WIN32 
        // This is to get rid of a trailing .exe 
        int exePos = exeName.find(".exe", 0);
        exeName = exeName.substr(0, exePos);
#endif

//         cout << "Exe name is " << exeName << endl;
        return exeName+".cfg";
    }

    // we are here if: there're multiple arguments but none of them are Ice.Config
    return std::string( "" );
}

std::string 
getServiceConfigFilename( const std::vector<std::string> & args )
{
    if ( args.empty() ) {
        return string();
    }

    // first look for Orca.Config parameter (specified by us manually)
    for ( unsigned int i=0; i<args.size(); ++i ) {
        if ( !args[i].compare( 0,13, "--Orca.Config" ) ) {
            return args[i].substr( 14, args[i].size()-1 );
        }
    }

    // now look for Ice.Config parameter (specified by IceGrid)
    for ( unsigned int i=0; i<args.size(); ++i ) {
        if ( !args[i].compare( 0,12, "--Ice.Config" ) ) {
            return args[i].substr( 13, args[i].size()-1 );
        }
    }

    // we are here if: there're multiple arguments but none of them are Ice.Config
    return string();
}

} // namespace
} // namespace
