/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef HYDRO_RANGERARRAY_PLAYERSONARCLIENT_H
#define HYDRO_RANGERARRAY_PLAYERSONARCLIENT_H

#include <hydrointerfaces/rangerarray.h>

// Player proxies
namespace PlayerCc
{
class PlayerClient;
class SonarProxy;
}

namespace rangerarrayplayersonarclient {

/*
    @brief Interface to PlayerSonar drivers.  Uses Player.

    @author Mikel Astiz (based on Alex Brooks' LaserScanner2dPlayerClient)
 */
class Driver : public hydrointerfaces::RangerArray
{
public:

    Driver( const hydroutil::Context& context );
    virtual ~Driver();
    
    virtual void getDescription( hydrointerfaces::RangerArray::Description &description );

    virtual void read( hydrointerfaces::RangerArray::Data &data );

private:

    void init();

    double detectionConeAngle_;
    bool usePlayerTimestamp_;

    PlayerCc::PlayerClient *robot_;
    PlayerCc::SonarProxy *sonar_;

    char *host_;
    int   port_;
    int   device_;

    hydroutil::Context context_;
};

// Used for dynamically loading driver
class Factory : public hydrointerfaces::RangerArrayFactory
{
public:
    hydrointerfaces::RangerArray*
    createDriver( const hydroutil::Context &context ) const
    {
        return new Driver( context );
    }
};

} // namespace

// Used for dynamically loading driver
extern "C" {
    hydrointerfaces::RangerArrayFactory *createDriverFactory();
}

#endif
