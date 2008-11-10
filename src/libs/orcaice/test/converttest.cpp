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

#include <orcaice/convertutils.h>
#include <orcaice/stringutils.h>

using namespace std;

int 
main(int argc, char * argv[])
{

    cout<<"testing toTopic() ... ";
    {
        orca::FQComponentName in;
        in.platform = "papa";
        in.component = "charlie";
        orca::FQTopicName out = orcaice::toTopic( in, "india", "tango" );
        orca::FQTopicName expect;
        expect.platform = in.platform;
        expect.component = in.component;
        expect.iface = "india";
        expect.topic = "tango";
        if ( out != expect ) {
            cout<<"failed"<<endl<<"got :"<<orcaice::toString( out ) <<endl;
            exit(EXIT_FAILURE);
        }
    }
    cout<<"ok"<<endl;

    exit(EXIT_SUCCESS);
}
