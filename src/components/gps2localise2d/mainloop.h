/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_FEATURE_EXTRACTOR_MAIN_LOOP_H
#define ORCA2_FEATURE_EXTRACTOR_MAIN_LOOP_H

#include <orcaice/thread.h>
#include <orcaice/proxy.h>
#include <orcaice/context.h>

#include <orca/localise2d.h>
#include <orca/gps.h>
#include <orcaifaceimpl/localise2diface.h>
#include <orcaifaceimpl/consumertypes.h>

namespace gps2localise2d
{

class Driver;

class MainLoop : public orcaice::Thread
{
public:

    MainLoop( orcaifaceimpl::Localise2dIface &localiseInterface,
              const orcaice::Context &context );

    ~MainLoop();

    virtual void run();

private:

    void connectToGps();
    void getGpsDescription();
    void initInterface();
    void initDriver();
    
    // This component is 2D-centric: can only handle certain orientations.
    bool antennaOffsetOK( const orca::Frame3d &antennaOffset );

    // generic algorithm
    Driver* driver_;
    
    // Our external interface
    orcaifaceimpl::Localise2dIface &localiseInterface_;
    
    orca::GpsPrx gpsPrx_;
    orca::Frame3d antennaOffset_;
    orca::GpsDescription gpsDescr_;

    // Consumers
    orcaifaceimpl::ProxiedGpsConsumerPtr gpsConsumer_;

    orcaice::Context context_;
};

} // namespace

#endif
