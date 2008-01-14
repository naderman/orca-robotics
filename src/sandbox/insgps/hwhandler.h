/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Michael Moser
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef HARDWARE_THREAD_H
#define HARDWARE_THREAD_H

#include <memory>
#include <hydroiceutil/eventqueue.h>
#include <orcaice/context.h>
#include <hydrodll/dynamicload.h>
// orca data types
#include <orca/ins.h>
#include <orca/gps.h>
#include <orca/imu.h>
// hardware interface
#include <hydrointerfaces/insgps.h>

namespace insgps {

//
// @brief the hardware loop of this component.
//
class HwHandler : public hydroiceutil::SubsystemThread
{

public:

    HwHandler( const orcaice::Context &context, hydroiceutil::EventQueuePtr dataPipe );

    // from SubsystemThread
    virtual void walk();

private:

    // Tries repeatedly to instantiate the driver
    void initHardwareDriver();

    hydrointerfaces::InsGps::Config config_;

    // used to shove data from the hardware side to the network side
    // event queue, so we can put through different data types
    hydroiceutil::EventQueuePtr dataPipe_;

    // The library that contains the driver factory (must be declared first so it's destructed last!!!)
    std::auto_ptr<hydrodll::DynamicallyLoadedLibrary> driverLib_;
    // Generic driver for the hardware
    std::auto_ptr<hydrointerfaces::InsGps> driver_;

    orcaice::Context context_;
};

enum EventType {
   OrcaIns,
   OrcaGps,
   OrcaImu
};

class OrcaInsEvent : public hydroiceutil::Event
{
    public:
        OrcaInsEvent(std::auto_ptr<hydrointerfaces::InsGps::InsData> hydroIns);
        orca::InsData data;
};
typedef IceUtil::Handle<OrcaInsEvent> OrcaInsEventPtr;

class OrcaGpsEvent : public hydroiceutil::Event
{
    public:
//        OrcaGpsEvent(const hydrointerfaces::InsGps::GpsData &hydroGps);
        OrcaGpsEvent(const std::auto_ptr<hydrointerfaces::InsGps::GpsData> hydroGps);
        orca::GpsData data;
};
typedef IceUtil::Handle<OrcaGpsEvent> OrcaGpsEventPtr;

class OrcaImuEvent : public hydroiceutil::Event
{
    public:
        OrcaImuEvent(std::auto_ptr<hydrointerfaces::InsGps::ImuData> hydroImu);
        orca::ImuData data;
};
typedef IceUtil::Handle<OrcaImuEvent> OrcaImuEventPtr;

} // namespace

#endif
