/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <sstream>

#include <IceUtil/Time.h>
#include <orcaice/orcaice.h>

#include "hwhandler.h"
#include "fakedriver.h"
// segway rmp drivers
#ifdef HAVE_USB_DRIVER
    #include "usb/rmpusbdriver.h"
#endif
#ifdef HAVE_PLAYERCLIENT_DRIVER
    #include "playerclient/playerclientdriver.h"
#endif

using namespace std;
using namespace orca;
using namespace segwayrmp;

HwHandler::HwHandler(
                 orcaice::PtrProxy<orca::Position2dDataPtr>    & position2dPipe,
                 orcaice::PtrProxy<orca::Position3dDataPtr>    & position3dPipe,
                 orcaice::PtrNotify<orca::Velocity2dCommandPtr>& commandPipe,
                 orcaice::PtrProxy<orca::PowerDataPtr>         & powerPipe,
                 orcaice::PtrProxy<orca::Platform2dConfigPtr>  & setConfigPipe,
                 orcaice::PtrProxy<orca::Platform2dConfigPtr>  & currentConfigPipe,
                 const orcaice::Context                        & context )
      : position2dPipe_(position2dPipe),
        position3dPipe_(position3dPipe),
        powerPipe_(powerPipe),
        setConfigPipe_(setConfigPipe),
        currentConfigPipe_(currentConfigPipe),
        position2dData_(new Position2dData),
        position3dData_(new Position3dData),
        powerData_(new PowerData),
        driver_(0),
        context_(context)
{
    // we'll handle incoming messages
    commandPipe.setNotifyHandler( this );

    // unsure about isOk until we enable the driver
    isOkProxy_.set( false );

    // set up data structure for 3 batteries
    // @todo Should this be done by the driver?
//     BatteryData bd;
//     for ( int i=0; i<3; ++i ) {
//         powerData_->batteries.push_back( bd );
//     }
    powerData_->batteries.resize(3);
    powerData_->batteries[0].name = "main-front";
    powerData_->batteries[1].name = "main-rear";
    powerData_->batteries[2].name = "ui";

    // this is the last place we can throw exceptions from.
    // after this the thread will be launched
    init();
}

HwHandler::~HwHandler()
{
    delete driver_;
}

void
HwHandler::init()
{
    //
    // Read settings
    //
    Ice::PropertiesPtr prop = context_.properties();
    std::string prefix = context_.tag() + ".Config.";
    
    config_.maxSpeed = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"MaxSpeed", 1.0 );
    config_.maxTurnrate = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"MaxTurnrate", 40.0 )*DEG2RAD_RATIO;
    config_.isMotionEnabled = (bool)orcaice::getPropertyAsIntWithDefault( prop, prefix+"EnableMotion", 1 );   

    // based on the config parameter, create the right driver
    string driverName = orcaice::getPropertyWithDefault( prop, prefix+"Driver", "segwayrmpusb" );
            
    if ( driverName == "segwayrmpusb" )
    {
#ifdef HAVE_USB_DRIVER
        context_.tracer()->debug( "loading 'segwayrmpusb' driver",3);
        driver_ = new RmpUsbDriver( context_ );
#else
        throw orcaice::Exception( ERROR_INFO, "Can't instantiate driver 'usb' because it was not built!" );
#endif
    }
    else if ( driverName == "playerclient" )
    {
#ifdef HAVE_PLAYERCLIENT_DRIVER
        context_.tracer()->debug( "loading 'playerclient' driver",3);
        driver_ = new PlayerClientDriver( context_ );
#else
        throw orcaice::Exception( ERROR_INFO, "Can't instantiate driver 'playerclient' because it was not built!" );
#endif
    }
    else if ( driverName == "fake" )
    {
        context_.tracer()->debug( "loading 'fake' driver",3);
        driver_ = new FakeDriver;
    }
    else {
        string errorStr = "Unknown driver type. Cannot talk to hardware.";
        context_.tracer()->error( errorStr);
        context_.tracer()->info( "Valid driver values are {'segwayrmpusb', 'playerclient', 'fake'}" );
        throw orcaice::Exception( ERROR_INFO, errorStr );
    }

    context_.tracer()->debug("driver instantiated",5);
}

void
HwHandler::enableDriver()
{
    while ( isActive() ) {
        context_.tracer()->info("Enabling driver...");
        if ( driver_->enable() == 0 ) 
        {
            context_.tracer()->info( "Enable succeeded." );
            return;
        }
        context_.tracer()->warning("failed to enable the driver; will try again in 2 seconds.");
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(2));
    }
}

void
HwHandler::run()
{
    std::string driverStatus = "";
    std::string currDriverStatus = "";
            
    while( isActive() )
    {
        try 
        {
            //
            // Make sure we're OK
            //
            bool isOk;
            isOkProxy_.get( isOk );
            if ( !isOk ) 
            {
                enableDriver();
                isOkProxy_.set( true );

                // we enabled, so presume we're OK.
                isOkProxy_.set( true );

                // but make sure we're not shutting down.
                if ( !isActive() )
                    break;
            }

            //
            // Read data from the hardware
            //
            int readStatus = driver_->read( position2dData_, position3dData_, powerData_, currDriverStatus );
    
            if ( readStatus==0 ) 
            {
                // Stick it in the buffer so pullers can get it
                position2dPipe_.set( position2dData_ );
                position3dPipe_.set( position3dData_ );
                powerPipe_.set( powerData_ );
                
                if ( driverStatus != currDriverStatus ) {
                    context_.tracer()->status( currDriverStatus );
                    driverStatus = currDriverStatus;
                }
            } 
            else 
            {
                context_.tracer()->error("failed to read data from Segway hardware.");
                isOkProxy_.set( false );
            }

        } // try
        catch ( const orca::OrcaException & e )
        {
            stringstream ss;
            ss << "unexpected (remote?) orca exception: " << e << ": " << e.what;
            context_.tracer()->error( ss.str() );
            if ( context_.isApplication() ) {
                context_.tracer()->info( "this is an stand-alone component. Quitting...");
                context_.communicator()->destroy();
            }
        }
        catch ( const orcaice::Exception & e )
        {
            stringstream ss;
            ss << "unexpected (local?) orcaice exception: " << e.what();
            context_.tracer()->error( ss.str() );
            if ( context_.isApplication() ) {
                context_.tracer()->info( "this is an stand-alone component. Quitting...");
                context_.communicator()->destroy();
            }
        }
        catch ( const Ice::Exception & e )
        {
            stringstream ss;
            ss << "unexpected Ice exception: " << e;
            context_.tracer()->error( ss.str() );
            if ( context_.isApplication() ) {
                context_.tracer()->info( "this is an stand-alone component. Quitting...");
                context_.communicator()->destroy();
            }
        }
        catch ( const std::exception & e )
        {
            stringstream ss;
            ss << "unexpected std exception: " << e.what();
            context_.tracer()->error( ss.str() );
            if ( context_.isApplication() ) {
                context_.tracer()->info( "this is an stand-alone component. Quitting...");
                context_.communicator()->destroy();
            }
        }
        catch ( ... )
        {
            context_.tracer()->error( "unexpected exception from somewhere.");
            if ( context_.isApplication() ) {
                context_.tracer()->info( "this is an stand-alone component. Quitting...");
                context_.communicator()->destroy();
            }
        }

    } // while

    // wait for the component to realize that we are quitting and tell us to stop.
    waitForStop();
    context_.tracer()->debug( "exiting HwHandler thread...",5);
}

//
// This is the only direct incoming link from the outside.
// Here we handle command arriving through Platform2d interface.
//
void
HwHandler::handleData( const orca::Velocity2dCommandPtr & obj )
{
    //cout<<"handling: "<<orcaice::toString(obj)<<endl;

/*
    // if we know we can't write, don't try again
    bool writeStatus = false;
    isOkProxy_.get( writeStatus );
    if ( !writeStatus ) {
        return;
    }
*/

    // check that platform motion is enabled
    if ( config_.isMotionEnabled==false ) {
        return;
    }

    // debug
    double msecs=writeTimer_.elapsed().toMilliSecondsDouble();
    writeTimer_.restart();
    // this will certainly be 'late' when we throw an exception below
    if ( msecs>300 ) {
        cout<<"late: " << msecs <<endl;
    }
    
    //
    // apply max limits
    //
    if ( fabs(obj->motion.v.x) > config_.maxSpeed ) {
        obj->motion.v.x =
                (obj->motion.v.x / fabs(obj->motion.v.x)) * config_.maxSpeed;
    }
    if ( fabs(obj->motion.w) > config_.maxTurnrate ) {
        obj->motion.w =
                (obj->motion.w / fabs(obj->motion.w)) * config_.maxTurnrate;
    }

    //
    // write to hardware
    //
    if( driver_->write( obj ) != 0 ) 
    {
        std::string errorStr = "failed to write command data to hardware.";
        context_.tracer()->error( errorStr );
        // set local state to failure
        isOkProxy_.set( false );

        // inform remote client of hardware failure
        throw orca::HardwareFailedException( errorStr );
    }
}
