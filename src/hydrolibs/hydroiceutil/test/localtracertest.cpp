/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <cstdlib>
#include <hydroiceutil/localtracer.h>

using namespace std;

int main(int argc, char * argv[])
{
    
    cout<<"testing LocalTracer() without props ..."<<endl;
    {
        hydroiceutil::LocalTracer tracer;

        int verb;
        verb = tracer.verbosity(gbxutilacfr::DebugTrace,gbxutilacfr::ToLog);
        if ( verb!=0 ) {
            cout<<"failed. wrong DebugToFile verbosity, expect=0, got="<<verb<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;

    cout<<"testing LocalTracer() with props ..."<<endl;
    hydroutil::Properties props;
    props.setProperty("InfoToDisplay",   "1" );
    props.setProperty("InfoToNetwork",   "2" );
    props.setProperty("InfoToLog",       "3" );
    props.setProperty("InfoToFile",      "4" );

    props.setProperty("WarningToDisplay","5" );
    props.setProperty("WarningToNetwork","6" );
    props.setProperty("WarningToLog",    "7" );
    props.setProperty("WarningToFile",   "8" );

    props.setProperty("ErrorToDisplay",  "9" );
    props.setProperty("ErrorToNetwork",  "10" );
    props.setProperty("ErrorToLog",      "11" );
    props.setProperty("ErrorToFile",     "12" );

    props.setProperty("DebugToDisplay",  "13" );
    props.setProperty("DebugToNetwork",  "14" );
    props.setProperty("DebugToLog",      "15" );
    props.setProperty("DebugToFile",     "16" );

    hydroiceutil::LocalTracer tracer( props, "test" );
    cout<<"ok"<<endl;

    cout<<"testing verbosity() ..."<<endl;
    {
        int verb;
        verb = tracer.verbosity(gbxutilacfr::InfoTrace,gbxutilacfr::ToDisplay);
        if ( verb!=1 ) {
            cout<<"failed. wrong InfoToDisplay verbosity, expect=1, got="<<verb<<endl;
            return EXIT_FAILURE;
        }
        verb = tracer.verbosity(gbxutilacfr::DebugTrace,gbxutilacfr::ToFile);
        if ( verb!=16 ) {
            cout<<"failed. wrong DebugToFile verbosity, expect=16, got="<<verb<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;

    return EXIT_SUCCESS;
}
