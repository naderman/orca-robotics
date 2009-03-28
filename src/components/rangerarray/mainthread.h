/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef MAIN_THREAD_H
#define MAIN_THREAD_H

#include <memory>
#include <orcaice/subsystemthread.h>
#include <orcaice/context.h>
#include <hydrodll/dynamicload.h>
// remote interface
#include <orcaifaceimpl/rangerarray.h>
// hardware interface
#include <hydrointerfaces/rangerarray.h>

namespace rangerarray {

class MainThread : public orcaice::SubsystemThread
{
public:
    MainThread( const orcaice::Context &context );

private:
    // from SubsystemThread
    virtual void initialise();
    virtual void work();
    virtual void finalise();

    // Tries repeatedly to instantiate the driver
    void initHardwareDriver();

    // Loops until established
    void initNetworkInterface();

    void readData();

    // The ranger array object
    orcaifaceimpl::RangerArrayImplPtr rangerArrayInterface_;

    orca::RangerArrayDescription       description_;

    // space for data
    orca::RangerArrayDataPtr           orcaRangerArrayData_;
    hydrointerfaces::RangerArray::Data hydroRangerArrayData_;
    
    // the following are internally needed too
    std::vector<int>                       auxDataTimeStampsSec_;
    std::vector<int>                       auxDataTimeStampsUsec_;

    // The library that contains the driver factory (must be declared first so it's destructed last!!!)
    std::auto_ptr<hydrodll::DynamicallyLoadedLibrary> driverLib_;
    // Generic driver for the hardware
    std::auto_ptr<hydrointerfaces::RangerArray> driver_;

    orcaice::Context context_;
};

} // namespace

#endif
