#include <hydroping/hydroping.h>
#include <cstdlib>
#include <iostream>

using namespace std;

int main( int argc, char **argv )
{
    try {
        cout<<"----------------------------------------"<<endl;
        cout<<"TRACE(pinglatencymstest.cpp): Pinging 'localhost' ..." << endl;

        const bool runWithSudo = true;
        const double latencyMs = hydroping::pingLatencyMs( "localhost",
                                                           runWithSudo,
                                                           "/opt/mrsrc/bin" );

        if ( latencyMs < 0 )
            cout<<"TRACE(pinglatencymstest.cpp): Timeout." << endl;
        else
            cout<<"TRACE(pinglatencymstest.cpp): Latency: " << latencyMs << "ms" << endl;
    } 
    catch ( std::exception &e )
    {
        cout << "TRACE(pinglatencymstest.cpp): Caught exception from call to pingLatencyMs: " << e.what() << endl;
    }

    try {
        cout<<"----------------------------------------"<<endl;
        cout<<"TRACE(pinglatencymstest.cpp): Pinging 'asdfasdf' ..." << endl;

        const bool runWithSudo = true;
        const double latencyMs = hydroping::pingLatencyMs( "asdfasdf",
                                                           runWithSudo,
                                                           "/opt/mrsrc/bin" );

        if ( latencyMs < 0 )
            cout<<"TRACE(pinglatencymstest.cpp): Timeout." << endl;
        else
            cout<<"TRACE(pinglatencymstest.cpp): Latency: " << latencyMs << "ms" << endl;
    } 
    catch ( std::exception &e )
    {
        cout << "TRACE(pinglatencymstest.cpp): Caught exception from call to pingLatencyMs: " << e.what() << endl;
    }

    return 0;
}
