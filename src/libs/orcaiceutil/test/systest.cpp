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

#include <orcaiceutil/sysutils.h>

using namespace std;

int 
main(int argc, char * argv[])
{
    cout<<"testing getHostname() ... ";
    // can't really test the output, just make sure it doesn't segfault and returns something
    std::string hostname = orcaiceutil::getHostname();
    if ( hostname == "" ) {
        cout<<"failed"<<endl<<"expect something; got="<<hostname<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;

    return EXIT_SUCCESS;
}
