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
#include <stdlib.h> // for getenv(), etc.

#include <orcaice/proputils.h>
#include <orcaice/exceptions.h>

using namespace std;

int 
main(int argc, char * argv[])
{
    std::string expect;
    std::string filename;
    Ice::StringSeq args;
    
    cout<<"testing getGlobalConfigFilename() with Orca.GlobalConfig ... ";
    Ice::PropertiesPtr prop = Ice::createProperties();
    prop->setProperty( "Orca.GlobalConfig", "orca.config_test" );
    expect = "orca.config_test";
    filename = orcaice::getGlobalConfigFilename( prop );
    if ( filename != expect ) {
        cout<<"failed"<<endl<<"expect="<<expect<<"; got="<<filename<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;

    cout<<"testing getGlobalConfigFilename() with ORCA_GLOBAL_CONFIG ... ";
    expect = "orca2_config_test";
#ifndef WIN32
    if ( setenv( "ORCA_GLOBAL_CONFIG", expect.c_str(), 1 ) ) {
        cout<<"failed to set ORCA_GLOBAL_CONFIG env variable."<<endl;
        return EXIT_FAILURE;
    }
#else
    // windows has to be different!
    // see http://msdn2.microsoft.com/en-us/library/tehxacec.aspx
    if ( _putenv_s( "ORCA_GLOBAL_CONFIG", expect.c_str() ) ) {
        cout<<"failed to set ORCA_GLOBAL_CONFIG env variable."<<endl;
        return EXIT_FAILURE;
    }
#endif
    Ice::PropertiesPtr emptyProps = Ice::createProperties();
    filename = orcaice::getGlobalConfigFilename( emptyProps );
    if ( filename != expect ) {
        cout<<"failed"<<endl<<"expect="<<expect<<"; got="<<filename<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;

    cout<<"testing getGlobalConfigFilename() with HOME ... ";
    // make sure ORCA_GLOBAL_CONFIG is not set
#ifndef WIN32
#if defined(__QNX__) || defined(__APPLE__)
	// the QNX version of unsetenv() returns void
    unsetenv( "ORCA_GLOBAL_CONFIG" );
#else
    if ( unsetenv( "ORCA_GLOBAL_CONFIG" ) ) {
        cout<<"failed to unset ORCA_GLOBAL_CONFIG env variable."<<endl;
        return EXIT_FAILURE;
    }
#endif
#else
    // windows does not have unsetenv() for some reason!
    if ( _putenv_s( "ORCA_GLOBAL_CONFIG", "" ) ) {
        cout<<"failed to set ORCA_GLOBAL_CONFIG env variable (actually want to unset)."<<endl;
        return EXIT_FAILURE;
    }
#endif
#ifndef WIN32
    expect = "home_test";
    if ( setenv( "HOME", expect.c_str(), 1 ) ) {
        cout<<"failed to set HOME env variable."<<endl;
        return EXIT_FAILURE;
    }
    filename = orcaice::getGlobalConfigFilename( emptyProps );
    if ( filename != (expect+"/.orcarc") ) {
        cout<<"failed"<<endl<<"expect="<<expect<<"/.orcarc"<<"; got="<<filename<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;
#else
    // in windows we don't use the HOME variable.
    filename = orcaice::getGlobalConfigFilename( emptyProps );
    if ( filename != "C:\\orca.ini" ) {
        cout<<"failed"<<endl<<"\texpect=C:\\orca.ini"<<"; got="<<filename<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;
#endif


    cout<<"testing getGlobalConfigFilename() with empty input ... ";
#ifndef WIN32
    // make sure HOME is not set
#if defined(__QNX__) || defined(__APPLE__)
	// the QNX version of unsetenv() returns void
    unsetenv( "HOME" );
#else
    if ( unsetenv( "HOME" ) ) {
        cout<<"failed to unset HOME env variable."<<endl;
        return EXIT_FAILURE;
    }
#endif
    try
    {
        filename = orcaice::getGlobalConfigFilename( emptyProps );
        cout<<"failed"<<endl<<"\texpect to throw exception with no input and no env variables :"<<filename<<endl;
        return EXIT_FAILURE;
    }
    catch ( const gbxsickacfr::gbxutilacfr::Exception & )
    {
        cout<<"ok"<<endl;
    }
#else
    // in windows we don't use the HOME variable.
    filename = orcaice::getGlobalConfigFilename( emptyProps );
    if ( filename != "C:\\orca.ini" ) {
        cout<<"failed"<<endl<<"\texpect=C:\\orca.ini"<<"; got="<<filename<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;
#endif

    cout<<"testing getApplicationConfigFilename() with Orca.Config ... ";
    expect = "orca.config_test";
    args.clear();
    args.push_back( "program" );
    args.push_back( "--Orca.Config="+expect );
    filename = orcaice::getApplicationConfigFilename( args );
    if ( filename != expect ) {
        cout<<"failed"<<endl<<"expect="<<expect<<"; got="<<filename<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;

    cout<<"testing getApplicationConfigFilename() with Ice.Config ... ";
    expect = "orca.config_test";
    args.clear();
    args.push_back( "program" );
    args.push_back( "--Ice.Config="+expect );
    filename = orcaice::getApplicationConfigFilename( args );
    if ( filename != expect ) {
        cout<<"failed"<<endl<<"expect="<<expect<<"; got="<<filename<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;

    cout<<"testing getApplicationConfigFilename() with mycomponent.cfg ... ";
    args.clear();
    expect = "mycomponent.cfg";
    args.push_back( "program" );
    args.push_back( expect );
    filename = orcaice::getApplicationConfigFilename( args );
    if ( filename != expect ) {
        cout<<"failed"<<endl<<"expect="<<expect<<"; got="<<filename<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;

    cout<<"testing getApplicationConfigFilename() with program name only ... ";
    args.clear();
    expect = "mycomponent.cfg";
    args.push_back( "mycomponent" );
    filename = orcaice::getApplicationConfigFilename( args );
    if ( filename != expect ) {
        cout<<"failed"<<endl<<"expect="<<expect<<"; got="<<filename<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;

    cout<<"testing getApplicationConfigFilename() with program path only ... ";
    args.clear();
    expect = "mycomponent.cfg";
#ifndef WIN32
    args.push_back( "/opt/orca/bin/mycomponent" );
#else
    args.push_back( "C:\\orca\\bin\\mycomponent" );
#endif
    filename = orcaice::getApplicationConfigFilename( args );
    if ( filename != expect ) {
        cout<<"failed"<<endl<<"\texpect="<<expect<<"; got="<<filename<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;

    cout<<"testing getApplicationConfigFilename() with empty input ... ";
    args.clear();
    filename = orcaice::getApplicationConfigFilename( args );
    expect = "";
    if ( filename != expect ) {
        cout<<"failed"<<endl<<"\texpect="<<expect<<"; got="<<filename<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;

    cout<<"testing getServiceConfigFilename() with Ice.Config ... ";
    expect = "orca.config_test";
    args.clear();
    args.push_back( "program" );
    args.push_back( "--Ice.Config="+expect );
    filename = orcaice::getServiceConfigFilename( args );
    if ( filename != expect ) {
        cout<<"failed"<<endl<<"expect="<<expect<<"; got="<<filename<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;

    cout<<"testing getServiceConfigFilename() without Ice.Config ... ";
    expect = "";
    args.clear();
    args.push_back( "program" );
    args.push_back( "--Orca.Bullshit=42" );
    try {
        filename = orcaice::getServiceConfigFilename( args );
        if ( filename != expect ) {
            cout<<"failed"<<endl<<"\texpect="<<expect<<"; got="<<filename<<endl;
            return EXIT_FAILURE;
        }
        cout<<"ok"<<endl;
    }
    catch ( const gbxsickacfr::gbxutilacfr::Exception & ) {
        cout<<"failed"<<endl<<"did not expect to catch an exception."<<endl;
        return EXIT_FAILURE;
    }

    cout<<"testing getServiceConfigFilename() with empty input ... ";
    try {
        args.clear();
        filename = orcaice::getServiceConfigFilename( args );
        cout<<"failed"<<endl<<"expected to catch an exception."<<endl;
        return EXIT_FAILURE;
    }
    catch ( const gbxsickacfr::gbxutilacfr::Exception & ) {
        cout<<"ok"<<endl;
    }

    return EXIT_SUCCESS;
}
