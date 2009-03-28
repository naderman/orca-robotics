#include <iostream>
#include <driverthread/driverthread.h>
#include <gbxutilacfr/trivialtracer.h>
#include <hydroiceutil/localstatus.h>
#include <hydroutil/context.h>
#include <segwayrmpacfr/driver.h>

using namespace std;
using namespace segwayrmpdriverthread;

class SimpleCallback : public Callback {
public:
    SimpleCallback( bool verbose )
        : verbose_(verbose)
        {}

    void hardwareInitialised()
        {
            cout << "SimpleCallback: hardware initialised" << endl;
        }

    void receiveData( const hydrointerfaces::SegwayRmp::Data &data )
        {
            cout<<"TRACE(segwaydriverthread.cpp): verbose_: " << verbose_ << endl;
            if ( verbose_ )
                cout << "SimpleCallback: data: " << endl << data.toString() << endl;
        }
private:
    bool verbose_;
};

const char *USAGE_ARGS="[-m(otor-enable)] [v(erbose)]";

int main( int argc, char **argv )
{
    DriverThread::Config config;
    config.driveInReverse = false;
    config.isMotionEnabled = false;
    config.maxForwardAcceleration = 1.0;
    config.maxReverseAcceleration = 1.0;
    config.stallSensorConfig.torqueThreshold = 3.0;
    config.stallSensorConfig.speedThreshold  = 0.5;
    config.stallSensorConfig.timeThreshold   = 0.5;

    hydroutil::Properties props;
    props.setProperty( "Acfr.AllowMoveInBalanceMode", "0" );
    props.setProperty( "Acfr.AllowMoveInTractorMode", "0" );
    props.setProperty( "Acfr.CanDevice", "/dev/pcan40" );

    // parse args
    bool motorEnable=false;
    bool verbose=false;
    for ( int i=1; i < argc; i++ )
    {
        if ( !strcmp( argv[i], "-m" ) )
        {
            motorEnable=true;
        }
        else if ( !strcmp( argv[i], "-v" ) )
        {
            verbose=true;
        }
        else
        {
            cout << "USAGE: " << argv[0] << " " << USAGE_ARGS << endl;
            exit(1);
        }
    }

    if ( motorEnable )
    {
        cout << endl;
        cout << "WARNING WARNING WARNING WARNING WARNING WARNING WARNING" << endl;
        cout << endl;
        cout << "Planning to enable the motors" << endl;
        cout << endl;
        cout << "  WHEELS WILL SPIN!" << endl;
        cout << endl;
        cout << "RMP wheels should be off the ground and free of obstructions." << endl;
        cout << endl;
        cout << " --> hit return to spin wheels" << endl;
        getchar();

        props.setProperty( "Acfr.AllowMoveInTractorMode", "1" );
        config.isMotionEnabled = true;
    }

    int debug=2, info=9, warn=9, error=9;
    gbxutilacfr::TrivialTracer tracer( debug, info, warn, error );
    hydroiceutil::LocalStatus status(tracer,props);
    hydroutil::Context context(props,tracer,status);

    segwayrmpacfr::Driver driver( context );
    SimpleCallback callback(verbose);

    DriverThread *driverThread = new DriverThread( config, 
                                                   driver,
                                                   tracer,
                                                   status, 
                                                   callback );

    gbxiceutilacfr::ThreadPtr driverThreadPtr = driverThread;
    driverThread->start();
    
    if ( !motorEnable )
    {
        sleep(3);
    }
    else
    {
        sleep(1);

        double secThisMode;
        const double periodSec=0.01;

        secThisMode=1.0;
        for ( int i=0; i < secThisMode/periodSec; i++ )
        {
            driverThread->setDesiredSpeed( hydrointerfaces::SegwayRmp::Command( 0.5, 0 ) );
            usleep( (int)(periodSec*1e6) );
        }
    }


    gbxiceutilacfr::stopAndJoin( driverThreadPtr );

    return 0;
}
