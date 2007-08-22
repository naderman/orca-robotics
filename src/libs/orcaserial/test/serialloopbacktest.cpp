#include <iostream>
#include <orcaserial/serial.h>
#include <string>
#include <sstream>
#include <vector>
#include <iomanip> 

using namespace std;

//
// A simple utility program for testing serial ports in loopback.
//

std::vector<std::string>
getStringList()
{
    std::vector<std::string> stringList;

    stringList.push_back( "string_one" );
    stringList.push_back( "string_two" );
    stringList.push_back( "string_three" );
    stringList.push_back( "string_four" );
    stringList.push_back( "" );
    stringList.push_back( "testing" );
    stringList.push_back( "asdfasdfasdfasdf" );

    return stringList;
}

int main( int argc, char **argv )
{
    if ( argc < 3 )
    {
        cout << "USAGE: " << argv[0] << " <dev> <baudrate>" << endl;
        exit(1);
    }

    const bool enableTimeouts = true;
    orcaserial::Serial serial( argv[1], atoi(argv[2]), enableTimeouts );
    serial.setTimeout( 1, 0 );

    const int NUM_CHARS = 1000;
    std::vector<std::string> stringList = getStringList();
    for ( uint i=0; i < NUM_CHARS; i++ )
    {
        int stringI = i % (stringList.size());
        std::string theString = stringList[stringI]+"\n";

        serial.writeString( theString );

        char buf[ theString.size()+1 ];
        int ret = serial.readLine( buf, theString.size()+1 );
        if ( ret < 0 )
        {
            cout << "ERROR(serialloopbacktest.cpp): Read timed out!" << endl;
            int nBytes = serial.bytesAvailable();
            cout<<"TRACE(serialloopbacktest.cpp): bytesAvailable: " << nBytes << endl;

            if ( nBytes == 0 )
            {
                cout<<"TRACE(serialloopbacktest.cpp): Ensure that the serial port is looped-back" << endl;
                cout<<"TRACE(serialloopbacktest.cpp): (ie pins 2 & 3 connected)" << endl;                
            }
            exit(1);
        }

        if ( !strcmp( buf, theString.c_str() ) )
        {
            cout<<"Wrote and read: " << theString << endl;
        }
        else
        {
            cout << "ERROR(serialloopbacktest.cpp): Strings didn't match!!" << endl;
            cout << "ERROR(serialloopbacktest.cpp): Wrote: '" << theString <<"'"<< endl;
            cout << "ERROR(serialloopbacktest.cpp): Read:  '" << buf <<"'"<< endl;

            cout<<"TRACE(serialloopbacktest.cpp): test FAILED" << endl;
            exit(1);
        }
    }

    cout<<"TRACE(serialloopbacktest.cpp): test PASSED" << endl;

    return 0;
}
