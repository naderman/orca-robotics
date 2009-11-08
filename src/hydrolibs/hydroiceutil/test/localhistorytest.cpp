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
#include <hydroiceutil/localhistory.h>

using namespace std;

int main(int argc, char * argv[])
{

    cout<<"testing LocalHistory() without props ..."<<endl;
    {
        hydroiceutil::LocalHistory history;
    }
    cout<<"ok"<<endl;

    cout<<"testing LocalHistory() with props ..."<<endl;
    {
        hydroutil::Properties props;
        props.setProperty("Enable",   "1" );
        hydroiceutil::LocalHistory history( props );
    }
    cout<<"ok"<<endl;

    cout<<"testing multiple progress reports ..."<<endl;
    {
        hydroutil::Properties props;
        props.setProperty("Enable",   "1" );
        hydroiceutil::LocalHistory history( props );
        history.report();
        history.report();
        history.report();
    }
    cout<<"ok"<<endl;

    cout<<"testing custom progress reports ..."<<endl;
    {
        hydroutil::Properties props;
        props.setProperty("Enable",   "1" );
        hydroiceutil::LocalHistory history( props );
        history.report( "100 200 300" );
        history.report( "101 201 301" );
        history.report( "102 202 302" );
    }
    cout<<"ok"<<endl;

    cout<<"testing mixed custom/standard progress reports ..."<<endl;
    {
        hydroutil::Properties props;
        props.setProperty("Enable",   "1" );
        hydroiceutil::LocalHistory history( props );
        history.report( "100 200 300" );
        history.report();
        history.report( "101 201 301" );
        history.report( "102 202 302" );
        history.report();
    }
    cout<<"ok"<<endl;

    return EXIT_SUCCESS;
}
