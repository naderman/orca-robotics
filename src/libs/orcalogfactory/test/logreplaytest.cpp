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

#include <orcaice/application.h>
#include <orcaice/component.h>
#include <orcaice/orcaice.h>
#include <orcalog/orcalog.h>


// all plug-ins
#include "../cameralogger.h"
#include "../cpulogger.h"
#include "../laserscanner2dlogger.h"
#include "../localise2dlogger.h"
#include "../polarfeature2dlogger.h"
#include "../powerlogger.h"

#include "../camerareplayer.h"
#include "../laserscanner2dreplayer.h"
#include "../powerreplayer.h"

using namespace std;

class TestComponent : public orcaice::Component
{
public:
    TestComponent() : orcaice::Component( "LogReplayTest" ) {};
    virtual ~TestComponent() {};

    // component interface
    virtual void start();
    virtual void stop() {};

private:
};

void 
TestComponent::start()
{
    try
    {

    orcalog::LogMaster* logMaster = 0;
    
    cout<<"*** Testing Camera log/replay ... "<<endl;
    {
        logMaster = new orcalog::LogMaster( "master.txt", context() );

        cout<<"Testing Camera logger with bad format ... "<<endl;
        try {
            orcalogfactory::CameraLogger( logMaster, "0", "horseshit", "", context() );
        }
        catch ( const orcalog::FormatNotSupportedException & ) {
            cout<<"ok"<<endl;
        }
        cout<<"Testing Camera replayer with bad format ... "<<endl;
        try {
            orcalogfactory::CameraReplayer( "horseshit", "camera0.log", context() );
        }
        catch ( const orcalog::FormatNotSupportedException & ) {
            cout<<"ok"<<endl;
        }

        cout<<"Testing Camera logger/replayer with 'ice' format ... ";
        orcalogfactory::CameraLogger* logger = 
            new orcalogfactory::CameraLogger( logMaster, "0", "ice", "", context() );
        orca::CameraData dataIn;
        orcaobj::setSane( dataIn, 20, 10 );
        logger->setData( dataIn, Ice::Current() );
    
        orcalogfactory::CameraReplayer* replayer = 
            new orcalogfactory::CameraReplayer( "ice", "camera0.log", context() );
        replayer->init( true );
        replayer->replayData( 0, true );  
        orca::CameraData dataOut = replayer->getData( Ice::Current() );
    
        if ( dataIn.imageWidth != dataOut.imageWidth
                || dataIn.compression != dataOut.compression
                || dataIn.image.size() != dataOut.image.size()
                || dataIn.image[7] != dataOut.image[7] ) {
            cout<<"failed"<<endl<<"object logged incorrectly"<<endl;
            cout<<"\tIN : "<<orcaice::toString(dataIn)<<endl;
            cout<<"\tOUT: "<<orcaice::toString(dataOut)<<endl;
            exit(EXIT_FAILURE);
        }

        delete logger;
        delete replayer;     
        delete logMaster;
    }
    cout<<"ok"<<endl;

    cout<<"*** Testing LaserScanner2d log/replay ... "<<endl;
    {
        logMaster = new orcalog::LogMaster( "master.txt", context() );

        orcalogfactory::LaserScanner2dLogger* logger = 
            new orcalogfactory::LaserScanner2dLogger( logMaster, "0", "ice", "", context() );
        orca::LaserScanner2dDataPtr dataIn = new orca::LaserScanner2dData;
        const orca::RangeScanner2dDataPtr rangeScanner2dDataIn = dataIn;
        orcaobj::setSane( dataIn );
        logger->setData( rangeScanner2dDataIn, Ice::Current() );
    
        orcalogfactory::LaserScanner2dReplayer* replayer = 
            new orcalogfactory::LaserScanner2dReplayer( "ice", "laserscanner2d0.log", context() );
        replayer->init( true );
        replayer->replayData( 0, true );  
        orca::RangeScanner2dDataPtr rangeScanner2dDataOut;
        rangeScanner2dDataOut = replayer->getData( Ice::Current() );
        orca::LaserScanner2dDataPtr dataOut = orca::LaserScanner2dDataPtr::dynamicCast(rangeScanner2dDataOut);    

        if ( dataIn->ranges[17] != dataOut->ranges[17] ) {
            cout<<"failed"<<endl<<"object logged incorrectly"<<endl;
            cout<<"\tIN : "<<orcaice::toString(dataIn)<<endl;
            cout<<"\tOUT: "<<orcaice::toString(dataOut)<<endl;
            exit(EXIT_FAILURE);
        }

        delete logger;
        delete replayer;     
        delete logMaster;
    }
    cout<<"ok"<<endl;

    cout<<"*** Testing Power log/replay 'ice' format ... "<<endl;
    {
        logMaster = new orcalog::LogMaster( "master.txt", context() );

        orcalogfactory::PowerLogger* logger = 
            new orcalogfactory::PowerLogger( logMaster, "0", "ice", "", context() );
        orca::PowerData dataIn;
        orcaobj::setSane( dataIn );
        logger->setData( dataIn, Ice::Current() );
    
        orcalogfactory::PowerReplayer* replayer = 
            new orcalogfactory::PowerReplayer( "ice", "power0.log", context() );
        replayer->init( true );
        replayer->replayData( 0, true );  
        orca::PowerData dataOut = replayer->getData( Ice::Current() );
    
        if ( dataIn.batteries.size() != dataOut.batteries.size()
                || dataIn.batteries[0].voltage != dataOut.batteries[0].voltage ) {
            cout<<"failed"<<endl<<"object logged incorrectly"<<endl;
            cout<<"\tIN : "<<orcaice::toString(dataIn)<<endl;
            cout<<"\tOUT: "<<orcaice::toString(dataOut)<<endl;
            exit(EXIT_FAILURE);
        }

        delete logger;
        delete replayer;     
        delete logMaster;
    }
    cout<<"ok"<<endl;

    cout<<"*** Testing Power log/replay 'ascii' format ... "<<endl;
    {
        logMaster = new orcalog::LogMaster( "master.txt", context() );
        orcalogfactory::PowerLogger* logger = 
            new orcalogfactory::PowerLogger( logMaster, "0", "ascii", "", context() );
        orca::PowerData dataIn;
        orcaobj::setSane( dataIn );
        logger->setData( dataIn, Ice::Current() );
    
        orcalogfactory::PowerReplayer* replayer = 
            new orcalogfactory::PowerReplayer( "ascii", "power0.log", context() );
        replayer->init( true );
        replayer->replayData( 0, true );  
        orca::PowerData dataOut = replayer->getData( Ice::Current() );
        
        if ( dataIn.batteries.size() != dataOut.batteries.size()
                || dataIn.batteries[0].voltage != dataOut.batteries[0].voltage ) {
            cout<<"failed"<<endl<<"object logged incorrectly"<<endl;
            cout<<"\tIN : "<<orcaice::toString(dataIn)<<endl;
            cout<<"\tOUT: "<<orcaice::toString(dataOut)<<endl;
            exit(EXIT_FAILURE);
        }

        delete logger;
        delete replayer;     
        delete logMaster;
    }
    cout<<"ok"<<endl;

    }
    catch ( const Ice::Exception& e ) {
        cout<<e<<endl;
        exit(EXIT_FAILURE);
    }
    catch ( const std::exception& e ) {
        cout<<e.what()<<endl;
        exit(EXIT_FAILURE);
    }
    catch ( ... ) {
        cout<<"TestComponent::start(): caught unknown exception."<<endl;
        exit(EXIT_FAILURE);
    }
    
    // NOTE: cannot call communicator()->destroy() from here
    // because they'll be caught by Ice::Application and show up as failed ctest.
    exit(EXIT_SUCCESS);
}

//
// Build the component into a stand-alone application
//
int 
main(int argc, char * argv[])
{
    TestComponent component;
    orcaice::Application app( component );
    return app.orcaMain(argc, argv);
}
