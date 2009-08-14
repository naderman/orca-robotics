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

// forward declarations
//
// avoid exposing the novatel gear to the world
namespace hydronovatelutil{
    struct BESTGPSPOS_LOG ;
    struct BESTGPSVEL_LOG ;
    struct INSPVASB_LOG ;
    struct RAWIMUSB_LOG ;
}
// users don't need to know about serial devices
namespace gbxserialacfr{
    class Serial;
}

namespace insgpsnovatelspan{

/*
@brief class which handles interactions with the
       Novatel Span InsGps navigation sensors

@author Matthew Ridley, Ben Upcroft, Michael Moser
 */
union novatel_message;
class Driver : public hydrointerfaces::InsGps
{
public:

    Driver( const Config &cfg, const hydroutil::Context &context );
    virtual ~Driver();

    // from InsGps
    virtual std::auto_ptr<hydrointerfaces::InsGps::GenericData> read();

private:

    //! establish a serial connection to the receiver
    int connectToHardware();
    //! transfer configuration parameters related to the IMU
    void configureImu();
    //! transfer configuration parameters related to GPS
    void configureGps();
    //! turn on data messages we are interested in
    void requestData();
    //! read one message from the receiver
    //! @param msg reference to the returned message
    //! returns the message's id (used to pick the right union member) or -1 on (CRC) error
    int readNovatelMessage( union novatel_message &msg );
    //! @brief convert the novatel datatypes into hydro format
    //! @{
    std::auto_ptr<hydrointerfaces::InsGps::GenericData> novatel2Hydro(hydronovatelutil::INSPVASB_LOG &insPva);
    std::auto_ptr<hydrointerfaces::InsGps::GenericData> novatel2Hydro(hydronovatelutil::BESTGPSPOS_LOG &bestGpsPos, hydronovatelutil::BESTGPSVEL_LOG &bestGpsVel);
    std::auto_ptr<hydrointerfaces::InsGps::GenericData> novatel2Hydro(hydronovatelutil::RAWIMUSB_LOG &rawImu);
    //! @}

    bool swappedRollPitch_;
    double undulation_; //aka goidal separation
    bool haveUndulation_;
    struct timeval timeOfRead_;

    std::auto_ptr<gbxserialacfr::Serial> serial_;
    int baud_;

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
