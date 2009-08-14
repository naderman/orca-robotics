/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Matthew Ridley, Ben Upcroft, Michael Moser
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef HYDRO_INSGPS_NOVATELSPAN_H
#define HYDRO_INSGPS_NOVATELSPAN_H

#include <memory>

#include <hydroutil/context.h>
#include <hydrointerfaces/insgps.h>

//#include <gbxnovatelacfr/driver.h>

// forward declarations
//
namespace gbxnovatelacfr{
    class BestGpsPosData;
    class BestGpsVelData;
    class Driver;
}

namespace insgpsnovatelgbx{

/*
@brief wrapper class for the GearBox Novatel Span InsGps navigation sensors

@author Michael Moser
 */
class Driver : public hydrointerfaces::InsGps
{
public:

    Driver( const Config &cfg, const hydroutil::Context &context );

    // from InsGps
    virtual std::auto_ptr<hydrointerfaces::InsGps::GenericData> read();

private:

    double undulation_; //aka goidal separation
    bool haveUndulation_;

    // temporary storage to assemble GpsData from velocity and position
    std::auto_ptr<gbxnovatelacfr::BestGpsPosData> bestGpsPosTmp_;
    std::auto_ptr<gbxnovatelacfr::BestGpsVelData> bestGpsVelTmp_;
    bool haveGpsPos_;
    bool haveGpsVel_;
    struct timeval timeOfGpsPos_;
    struct timeval timeOfGpsVel_;

    std::auto_ptr<gbxnovatelacfr::Driver > device_;
    hydroutil::Context context_;
};


// Used for dynamically loading driver
class Factory : public hydrointerfaces::InsGpsFactory
{
public:
    hydrointerfaces::InsGps *createDriver( 
                const hydrointerfaces::InsGps::Config &config,
                const hydroutil::Context &context ) const
    {
        return new Driver( config, context );
    }
};

} // namespace

// Used for dynamically loading driver
extern "C" {
    hydrointerfaces::InsGpsFactory *createDriverFactory();
}
#endif
