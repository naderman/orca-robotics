#include <hydroping/hydroping.h>
#include <cstdlib>
#include <iostream>

using namespace std;
using namespace hydroping;

int main( int argc, char **argv )
{
    if ( argc < 2 )
    {
        cout << "Usage: pingtest [host]" << endl;
        exit(1);
    }
    
    try {
        Pinger pinger;
        cout<<"TRACE(pingtest.cpp): Pinging " << argv[1] << "..." << endl;
        double latencyMs = pinger.ping( argv[1] );

        if ( latencyMs < 0 )
            cout << "Timeout." << endl;
        else
            cout << "latency: " << latencyMs << "ms" << endl;
    } 
    catch ( PingException &e )
    {
        cout << "ERROR(pingtest.cpp): Error: " << e.what() << endl;
        exit(1);
    }
    return 0;
}
