#include <iostream>
#include "segwayrmp.h"
#include <cmath>
#include <stdio.h>
#include <gbxutilacfr/trivialtracer.h>
#include <hydroiceutil/localstatus.h>
#include <hydrodrivers/humaninput2dkbdtermio/driver.h>

using namespace std;

//
// Callback to receive odometry etc from the RMP
//
class Callback : public orcarmputil::AggregatorCallback {
public:
    void hardwareInitialised( int powerbaseID )
        {
            cout << __func__ << "(powerbaseID="<<powerbaseID<<")"<<endl;
        }
    void receiveData( int                                     powerbaseID,
                      const hydrointerfaces::SegwayRmp::Data &data )
        {
            if ( powerbaseID != 0 )
            {
                // Ignore (we just watch a single powerbase for the odometry)
                return;
            }
//            cout << __func__ << "(): data="<<data<<endl;
        }
};

int main()
{
    //
    // Set up some configuration
    //
    hydrointerfaces::SegwayRmp::Capabilities capabilitiesConstraints;
    capabilitiesConstraints.maxForwardSpeed        = 1.0;
    capabilitiesConstraints.maxReverseSpeed        = 1.0;
    capabilitiesConstraints.maxTurnrate            = 60.0*M_PI/180.0;
    capabilitiesConstraints.maxLateralAcceleration = 1.0;
    cout<<"user-defined capabilitiesConstraints: " << capabilitiesConstraints << endl;

    const double maxForwardAcceleration = 1.0;
    const double maxReverseAcceleration = maxForwardAcceleration;
    cout << "maxForwardAcceleration: " << maxForwardAcceleration << endl
         << "maxReverseAcceleration: " << maxReverseAcceleration << endl;

    const bool warnDefaultProperty = false;
    hydroutil::Properties prop( warnDefaultProperty );
    prop.setProperty( "PowerbaseNames", "Front Rear" );
    prop.setProperty( "EnableMotion", "1" );
    prop.setProperty( "Front.Acfr.CanDevice", "/dev/pcan40" );
    prop.setProperty( "Rear.Acfr.CanDevice", "/dev/pcan41" );
    prop.setProperty( "Front.Acfr.ProductModel", "RMP400" );
    prop.setProperty( "Rear.Acfr.ProductModel", "RMP400" );

    gbxutilacfr::TrivialTracer tracer;
    hydroiceutil::LocalStatus status(tracer,prop);
    hydroutil::Context context( prop, tracer, status );

    //
    // Set up comms to the segway
    //
    Callback callback;
    const bool fakeDriver = false;
    segwayrmp::SegwayRmp segwayRmp( capabilitiesConstraints,
                                    maxForwardAcceleration,
                                    maxReverseAcceleration,
                                    callback,
                                    fakeDriver,
                                    context );

    //
    // Set up comms to the human
    //
    const double linearStep = 0.05;
    const double rotationalStep = 5.0*M_PI/180.0;
    std::auto_ptr<hydrointerfaces::HumanInput2d> humanInput(
        new humaninput2dkbdtermio::Driver( context ) );
    humanInput->enable();

   // Read commands from the human in a loop
   hydrointerfaces::HumanInput2d::Command humanCommand;
   hydrointerfaces::SegwayRmp::Command    setPoint(0,0);   
   while ( true )
   {
       try {
           int retVal = humanInput->read( humanCommand );
           if ( retVal == 0 )
           {
               if ( humanCommand.isLongIncrement )
                   setPoint.vx += humanCommand.longitudinal*linearStep;
               else
                   setPoint.vx = humanCommand.longitudinal;
           
               if ( humanCommand.isAngularIncrement )
                   setPoint.w += humanCommand.angular*rotationalStep;
               else
                   setPoint.w = humanCommand.angular;
               cout<<endl<<"New set-point: " << setPoint << endl;
           }
           else
           {
               cout << ".";
               fflush(stdout);
           }
           segwayRmp.setCommand( setPoint );
       }
       catch ( std::exception &e )
       {
           cout << e.what() << endl;
       }
   }
   humanInput->disable();

    return 0;
}
