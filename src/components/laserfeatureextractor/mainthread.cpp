/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp,
 *               George Mathews, Stef Williams.
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>
#include <orcaobj/orcaobj.h>
#include "mainthread.h"
#include "combineddriver.h"

using namespace std;
using namespace laserfeatures;

MainThread::MainThread( const orcaice::Context &context ) :
    SubsystemThread( context.tracer(), context.status(), "MainThread" ),
    context_(context)
{
    subStatus().setMaxHeartbeatInterval( 10.0 );

    // create a callback object to recieve scans
    laserConsumer_ = new orcaifaceimpl::StoringRangeScanner2dConsumerImpl( context_ );
}

void 
MainThread::initDriver()
{
    std::string prefix = context_.tag() + ".Config.";
    Ice::PropertiesPtr prop = context_.properties();

    std::string driverName = orcaice::getPropertyWithDefault( prop,
            prefix+"Driver", "combined" );
    
    if ( driverName == "combined" )
    {
        context_.tracer().debug( "loading 'combined' driver",3);
        driver_.reset( new CombinedDriver( laserDescr_, context_ ) );
    }
    else
    {
        std::string errString = "Unknown algorithm driver: " + driverName;
        context_.tracer().error( errString );
        exit(1);
    }
    
    context_.tracer().debug("driver instantiated",5);
}

void 
MainThread::connectToLaser()
{
    while ( !isStopping() )
    {
        try
        {
            context_.tracer().debug( "Connecting to laser...", 3 );
            orcaice::connectToInterfaceWithTag<orca::LaserScanner2dPrx>( 
                context_, laserPrx_, "Laser" );
            context_.tracer().debug("connected to a 'Laser' interface", 4 );
            break;
        }
        catch ( const Ice::Exception &e )
        {
            stringstream ss;
            ss << "failed to connect to laser interface: " << e;
            context_.tracer().error( ss.str() );
        }
        catch ( const std::exception &e )
        {
            stringstream ss;
            ss << "failed to connect to laser interface: " << e.what();
            context_.tracer().error( ss.str() );
        }
        catch ( ... )
        {
            context_.tracer().error( "Failed to connect to laser interface for unknown reason." );
        }
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(2));
    }

    while ( !isStopping() )
    {
        try
        {
            context_.tracer().debug( "Subscribing to laser...", 3 );
            laserPrx_->subscribe( laserConsumer_->consumerPrx() );
            break;
        }
        catch ( const Ice::Exception &e )
        {
            stringstream ss;
            ss << "failed to subscribe to laser: " << e;
            context_.tracer().error( ss.str() );
        }
        catch ( const std::exception &e )
        {
            stringstream ss;
            ss << "failed to subscribe to laser: " << e.what();
            context_.tracer().error( ss.str() );
        }
        catch ( ... )
        {
            context_.tracer().error( "Failed to subscribe to laser for unknown reason." );
        }
        subStatus().initialising("connectToLaser()" );
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(2));
    }

    context_.tracer().info( "Connected to laser." );
}

void 
MainThread::getLaserDescription()
{
    while ( !isStopping() )
    {
        try
        {
            context_.tracer().debug( "Getting laser description...", 2 );
            laserDescr_ = laserPrx_->getDescription();
            stringstream ss;
            ss << "Got laser description: " << orcaobj::toString( laserDescr_ );
            context_.tracer().info( ss.str() );
        }
        catch ( const Ice::Exception &e )
        {
            stringstream ss;
            ss << "failed to retreive laser description: " << e;
            context_.tracer().error( ss.str() );
        }
        catch ( const std::exception &e )
        {
            stringstream ss;
            ss << "failed to retreive laser description: " << e.what();
            context_.tracer().error( ss.str() );
        }
        catch ( ... )
        {
            context_.tracer().error( "Failed to retreive laser description for unknown reason." );
        }
        subStatus().initialising("getLaserDescription()" );
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(2));
    }
}

void
MainThread::initNetworkInterface()
{    
    //
    // Instantiate External Interface
    //
    orca::PolarFeature2dDescription descr;
    descr.timeStamp = orcaice::getNow();
    descr.offset = laserDescr_.offset;
    featureInterface_ = new orcaifaceimpl::PolarFeature2dImpl( descr, "PolarFeature2d", context_ );

    // init
    featureInterface_->initInterface( this, subsysName() );
}

void 
MainThread::walk()
{
    // These functions catch their exceptions.
    activate( context_, this, subsysName() );

    connectToLaser();
    getLaserDescription();
    initDriver();
    initNetworkInterface();

    orca::PolarFeature2dData featureData;

    orca::LaserScanner2dDataPtr laserData;
    orca::RangeScanner2dDataPtr rangeData;

    // wake up every now and then to check if we are supposed to stop
    const int timeoutMs = 1000;

    context_.tracer().debug( "Entering main loop.",2 );
    subStatus().setMaxHeartbeatInterval( 2.0 );

    // Loop forever till we get shut down.
    while ( !isStopping() )
    {
        // this try makes this component robust to exceptions
        try
        {                
            //
            // block on arrival of laser data
            //
            int ret = laserConsumer_->store().getNext( rangeData, timeoutMs );
            if ( ret != 0 ) {
                stringstream ss;
                ss << "Timed out (" << timeoutMs << "ms) waiting for laser data.  Reconnecting.";
                context_.tracer().warning( ss.str() );
                subStatus().warning( ss.str() );
                connectToLaser();
                continue;
            }

            // Assume that what we're really getting is laser data
            laserData = orca::LaserScanner2dDataPtr::dynamicCast( rangeData );
            
            if ( (int)(laserData->ranges.size()) != laserDescr_.numberOfSamples )
            {
                stringstream ss;
                ss << "Got laser scan: expected " << laserDescr_.numberOfSamples
                    << " returns, got " << laserData->ranges.size();
                context_.tracer().warning( ss.str() );
                subStatus().warning( ss.str() );
                continue;
            }

            // cout << "INFO(algorithmhandler.cpp): Getting laserData of size "
            //      << laserData->ranges.size() << " from proxy" << endl << endl;

            //
            // execute algorithm to compute features
            //
            driver_->computeFeatures( laserData, featureData );

            // features have the same time stamp as the raw scan
            featureData.timeStamp = laserData->timeStamp;

            featureInterface_->localSetAndSend( featureData );

            subStatus().ok();
        } // try
        catch ( const orca::OrcaException & e )
        {
            std::stringstream ss;
            ss << "MainThread: unexpected orca exception: " << e << ": " << e.what;
            context_.tracer().error( ss.str() );
        }
        catch ( const Ice::Exception & e )
        {
            std::stringstream ss;
            ss << "MainThread: unexpected Ice exception: " << e;
            context_.tracer().error( ss.str() );
        }
        catch ( const std::exception & e )
        {
            std::stringstream ss;
            ss << "MainThread: unexpected std exception: " << e.what();
            context_.tracer().error( ss.str() );
        }
        catch ( const std::string &e )
        {
            std::stringstream ss;
            ss << "MainThread: unexpected std::string exception: " << e;
            context_.tracer().error( ss.str() );
        }
        catch ( ... )
        {
            context_.tracer().error( "MainThread: unexpected exception from somewhere.");
        }
    } // while

    context_.tracer().debug( "Exited main loop.",2 );
}

#if 0
void
convertPointToRobotCS( double &range,
                       double &bearing,
                       const orca::CartesianPoint &offsetXyz,
                       const orca::OrientationE offsetAngles )
{
    orca::CartesianPoint LaserXy, RobotXy;
    
    LaserXy.x = cos(bearing) * range;
    LaserXy.y = sin(bearing) * range;
    RobotXy.x = LaserXy.x*cos(offsetAngles.y) - LaserXy.y*sin(offsetAngles.y) + offsetXyz.x;
    RobotXy.y = LaserXy.x*sin(offsetAngles.y) + LaserXy.y*cos(offsetAngles.y) + offsetXyz.y;
    range   = sqrt(RobotXy.x*RobotXy.x + RobotXy.y*RobotXy.y);
    bearing = atan2(RobotXy.y,RobotXy.x);
}

void 
MainThread::convertToRobotCS( const orca::PolarFeature2dData &featureData )
{
    orca::CartesianPoint offsetXyz = sensorOffset_.p;
    orca::OrientationE   offsetAngles = sensorOffset_.o;
    
    for (unsigned int i=0; i<featureData.features.size(); i++ )
    {
        // a bit ugly...
        orca::SinglePolarFeature2dPtr ftr = featureData.features[i];
        if ( ftr->ice_isA( "::orca::PointPolarFeature2d" ) )
        {
            orca::PointPolarFeature2d& f = dynamic_cast<orca::PointPolarFeature2d&>(*ftr);
            assert( f.p.r >= 0 && f.p.r < laserDescr_.maxRange );
            assert( f.p.o > laserDescr_.startAngle && 
                    f.p.o < laserDescr_.startAngle+laserDescr_.fieldOfView );
            assert( f.rangeSd >= 0 && f.bearingSd >= 0 );
            convertPointToRobotCS( f.p.r, f.p.o, offsetXyz, offsetAngles );
        }
        else if ( ftr->ice_isA( "::orca::LinePolarFeature2d" ) )
        {
            orca::LinePolarFeature2d& f = dynamic_cast<orca::LinePolarFeature2d&>(*ftr);

            // Check a few things...
            // (Allow slack because funny things can happen as endpoints are projected)
            const bool EPSR=0.5;
            const bool EPSB=5*M_PI/180.0;
            if ( f.startSighted )
            {
                assert( f.start.r >= -EPSR && f.start.r < laserDescr_.maxRange+EPSR );
                assert( f.start.o > laserDescr_.startAngle-EPSB && 
                        f.start.o < laserDescr_.startAngle+laserDescr_.fieldOfView+EPSB );
            }
            if ( f.endSighted )
            {
                assert( f.end.r >= -EPSR && f.end.r < laserDescr_.maxRange+EPSR );
                assert( f.end.o > laserDescr_.startAngle-EPSB && 
                        f.end.o < laserDescr_.startAngle+laserDescr_.fieldOfView+EPSB );
            }
            assert ( f.rhoSd >= 0 && f.alphaSd >= 0 );

            //
            // Check: the line should be visible.
            //        (prior to conversion to robot-centric CS)
            // 
            double bearingDiff = f.end.o - f.start.o;
            if ( bearingDiff < 0 )
            {
                stringstream ss;
                ss << "MainThread::convertToRobotCS(): bearingDiff < 0 -- line is not visible from sensor pose!"
                   << "  Line was: " 
                   << orcaobj::toString(ftr);
                throw gbxsickacfr::gbxutilacfr::Exception( ERROR_INFO, ss.str() );
            }

            convertPointToRobotCS( f.start.r, f.start.o, offsetXyz, offsetAngles );
            convertPointToRobotCS( f.end.r,   f.end.o,   offsetXyz, offsetAngles );

            //
            // Check: the line should be visible.
            //        (after conversion to robot-centric CS)
            // 
            bearingDiff = f.end.o - f.start.o;
            if ( bearingDiff < 0 )
            {
                //
                // Line is not visible from platform pose (but it was visible from sensor pose -- 
                //   this can happen if the two are not co-located.
                //   So turn the thing around.
                //
                stringstream ss;
                ss << "MainThread::convertToRobotCS(): bearingDiff < 0 -- line is not visible from platform pose!"
                   << "  (this is possible when sensor/platforms poses are not co-located).  Ignoring."<<endl
                   << "  Line was: " 
                   << orcaobj::toString(ftr);
                throw gbxsickacfr::gbxutilacfr::Exception( ERROR_INFO, ss.str() );
            }
        }
        else
        {
            stringstream ss; ss<<"ERROR(mainloop.cpp): Unknown feature type: " << ftr->type;
            throw ss.str();
        }
    }
}

bool
MainThread::sensorOffsetOK( const orca::Frame3d & offset )
{
    bool offsetOk = true;
    if ( offset.p.z != 0.0 )
    {
        stringstream ss;
        ss << "Can't handle non-zero 'z' component in laser offset. Offset: " << orcaobj::toString(offset);
        context_.tracer().error( ss.str() );
        offsetOk = false;
    }
    if ( offset.o.r != 0.0 || offset.o.p != 0.0 )
    {
        stringstream ss;
        ss << "Can't handle non-zero roll or pitch in laser offset. Offset: " << orcaobj::toString(offset);
        context_.tracer().error( ss.str() );
        offsetOk = false;
    }

    return offsetOk;
}
#endif
