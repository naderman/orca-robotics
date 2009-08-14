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
#include   <cstdlib>
#include <hydroutil/sysutils.h>

using namespace std;

int 
main(int argc, char * argv[])
{
    cout<<"testing getHostname() ... ";
    // can't really test the output, just make sure it doesn't segfault and returns something
    std::string hostname = hydroutil::getHostname();
    if ( hostname == "" ) {
        cout<<"failed"<<endl<<"expect something; got="<<hostname<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;

    return EXIT_SUCCESS;
}
