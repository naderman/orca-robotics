/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Mathew Ridley, Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_GPS_COMPONENT_H
#define ORCA2_GPS_COMPONENT_H

#include <orcaice/component.h>
#include <orcaice/ptrbuffer.h>
#include <orca/bros1.h>
#include <orca/gps.h>

class GpsHandler;
class GpsDriver;

class GpsComponent : public orcaice::Component
{
public:

    GpsComponent();
    virtual ~GpsComponent();

    virtual void start();
    virtual void stop();

private:

    GpsHandler *handler_;
    orca::GpsDescription descr_;

    //
    // EXTERNAL INTERFACE: Gps
    //
    Ice::ObjectPtr gpsObjPtr_;

    //
    // HARDWARE INTERFACES
    //
    GpsDriver *hwDriver_;
};

#endif
