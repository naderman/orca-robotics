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
#include <orcaice/orcaice.h>
#include <orcaifaceutil/ins.h>
#include <orcaifaceutil/gps.h>
#include <orcaobj/orcaobj.h> // for getPropertyAs...()
#include "hwthread.h"
#include "mainthread.h"

using namespace std;
using namespace insgps;


MainThread::MainThread( const orcaice::Context &context ) :
    orcaice::SubsystemThread( context.tracer(), context.status(), "MainThread" ),
    hwThread_(new HwThread(context)),
    context_(context)
{
}

MainThread::~MainThread()
{
    context_.tracer().debug( "stopping hwThread", 5 );
    gbxiceutilacfr::stopAndJoin( hwThread_ );
    context_.tracer().debug( "stopped hwThread", 5 );
    return;
}

void
MainThread::initNetworkInterface()
{
    Ice::PropertiesPtr prop = context_.properties();
    std::string prefix = context_.tag() + ".Config.";

    // SENSOR DESCRIPTION
    orca::InsDescription insDescr;
    orca::GpsDescription gpsDescr;
    orca::ImuDescription imuDescr;

    insDescr.timeStamp = orcaice::getNow();
    gpsDescr.timeStamp = orcaice::getNow();
    imuDescr.timeStamp = orcaice::getNow();

    // transfer internal sensor configs
    ifaceutil::zeroAndClear(insDescr.offset);
    ifaceutil::zeroAndClear(gpsDescr.antennaOffset);
    ifaceutil::zeroAndClear(imuDescr.offset);
    ifaceutil::zeroAndClear(imuDescr.size);

    insDescr.offset = orcaobj::getPropertyAsFrame3dWithDefault( prop, prefix+"Ins.Offset", insDescr.offset );
    gpsDescr.antennaOffset = orcaobj::getPropertyAsFrame3dWithDefault( prop, prefix+"Gps.AntennaOffset", gpsDescr.antennaOffset );
    imuDescr.offset = orcaobj::getPropertyAsFrame3dWithDefault( prop, prefix+"Imu.Offset", imuDescr.offset );
    imuDescr.size = orcaobj::getPropertyAsSize3dWithDefault( prop, prefix+"Imu.Size", imuDescr.size );

    // EXTERNAL PROVIDED INTERFACES
    insInterface_ = new orcaifaceimpl::InsImpl( insDescr, "Ins", context_ );
    gpsInterface_ = new orcaifaceimpl::GpsImpl( gpsDescr, "Gps", context_ );
    imuInterface_ = new orcaifaceimpl::ImuImpl( imuDescr, "Imu", context_ );

    insInterface_->initInterface( this, subsysName() );
    gpsInterface_->initInterface( this, subsysName() );
    imuInterface_->initInterface( this, subsysName() );
}

void
MainThread::walk()
{
    subStatus().initialising();
    subStatus().setMaxHeartbeatInterval( 20.0 );

    // These functions catch their exceptions.
    activate( context_, this, subsysName() );
    initNetworkInterface();
    hwThread_->start();

    subStatus().working();

    while(!isStopping())
    {
        try
        {
            hydroiceutil::EventPtr e;

            // this blocks until new data arrives
            if(true == hwThread_->dataPipe_->timedGet(e,1000)){
                // determine what we've got and shove it of to IceStorm
                switch(e->type()){
                    case OrcaIns:
                        insInterface_->localSetAndSend((OrcaInsEventPtr::dynamicCast(e))->data);
                        break;
                    case OrcaGps:
                        gpsInterface_->localSetAndSend((OrcaGpsEventPtr::dynamicCast(e))->data);
                        break;
                    case OrcaImu:
                        imuInterface_->localSetAndSend((OrcaImuEventPtr::dynamicCast(e))->data);
                        break;
                    default:
                        break;
                }
            }else{
                subStatus().warning( "timed out read on hwThread" );
            }
            continue;
        } // end of try
        catch ( ... ) 
        {
            orcaice::catchMainLoopExceptions( subStatus() );
        }

    } // end of while
}

