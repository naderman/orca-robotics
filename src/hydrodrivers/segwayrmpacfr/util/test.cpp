#include <iostream>
#include <driver.h>
#include <hydroutil/properties.h>
#include <hydroiceutil/localtracer.h>
#include <gbxutilacfr/trivialtracer.h>
#include <hydroiceutil/localstatus.h>
#include <stdlib.h>

using namespace std;

int main( int argc, char **argv )
{
    std::string portName = "/dev/pcan40";

    const std::string USAGE_ARGS = "[-p <portName>]";
    for ( int i=1; i < argc; i++ )
    {
        if ( !strcmp(argv[i],"-p") &&
             i+1 < argc )
        {
            portName = argv[i+1];
            i++;
        }
        else
        {
            cout << "USAGE: " << USAGE_ARGS;
            exit(1);
        }
    }

    hydroutil::Properties props;

    // CAN settings
    props.setProperty( "Acfr.CanDevice", portName );

    int debug=9, info=3, warn=3, error=3;
    gbxutilacfr::TrivialTracer tracer( debug, info, warn, error );
    hydroiceutil::LocalStatus status( tracer, props );

    hydroutil::Context context( props, tracer, status );

    segwayrmpacfr::Driver driver( "TestPowerbase", context );

    while ( true )
    {
        try {
            cout<<"TRACE(test.cpp): Enabling driver" << endl;
            while ( true )
            {
                try {
                    driver.enable();
                    break;
                }
                catch ( std::exception &e )
                {
                    cout<<"TRACE(test.cpp): Caught exception trying to enable: " << e.what() << endl;
                    sleep(1);
                }
            }

            cout<<"TRACE(test.cpp): reading from driver ... (hit Ctrl-C to exit)" << endl;
            hydrointerfaces::SegwayRmp::Data data;
            while ( true )
            {
                driver.read(data);
                if ( data.hasWarnings || data.hasFaults )
                {
                    cout << "warnings/faults: " << data.warnFaultReason << endl;
                }
            }
        }
        catch ( std::exception &e )
        {
            cout<<"TRACE(test.cpp): " << e.what() << endl;
        }
    }

    return 0;
}
