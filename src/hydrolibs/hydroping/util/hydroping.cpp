#include <hydroping/hydroping.h>
#include <cstdlib>
#include <iostream>
#include <string.h>
#include <sstream>

using namespace std;
using namespace hydroping;

int main( int argc, char **argv )
{
    std::stringstream usageArgsSS;
    usageArgsSS << "<host> [--parseable]" << endl
                << "  Where: '--parseable' prints:"<<endl
                << "      - the latency in ms (or '-1' for timeout) [returns 0] normally or" << endl
                << "      - \"ERR: <errorDescription>\" [returns non-zero] on error";
    const std::string USAGE_ARGS = usageArgsSS.str();

    if ( argc < 2 )
    {
        cout << "USAGE: "<<USAGE_ARGS<<endl;
        exit(1);
    }
    
    bool parseableOutput=false;
    for ( int argI=2; argI < argc; argI++ )
    {
        if ( !strcmp(argv[argI],"--parseable") )
        {
            parseableOutput = true;
            argI++;
        }
        else
        {
            cout << "USAGE: "<<USAGE_ARGS<<endl;
            exit(1);            
        }
    }

    try {
        Pinger pinger;
        double latencyMs = pinger.ping( argv[1] );
        if ( parseableOutput )
        {
            cout << latencyMs << endl;
        }
        else
        {
            if ( latencyMs < 0 )
                cout << "Timeout." << endl;
            else
                cout << "latency: " << latencyMs << "ms" << endl;
        }
    } 
    catch ( PingException &e )
    {
        if ( parseableOutput )
        {
            cout << "ERR: " << e.what() << endl;;
        }
        else
        {
            cout << "Error: " << e.what() << endl;
        }
        exit(1);
    }
    return 0;
}
