/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef HYDRO_SEGWAYRMP_PLAYER_CLIENT_H
#define HYDRO_SEGWAYRMP_PLAYER_CLIENT_H

#include <hydrointerfaces/segwayrmp.h>

// Player proxies
namespace PlayerCc
{
class PlayerClient;
class Position2dProxy;
class Position3dProxy;
class PowerProxy;
}

namespace segwayrmpplayerclient
{

class Driver : public hydrointerfaces::SegwayRmp
{
public:

    Driver( const hydroutil::Context& context );
    virtual ~Driver();

    // from SegwayRmp
    virtual void enable();
    virtual void read( Data& data );
    virtual void write( const Command& command );
    virtual hydrointerfaces::SegwayRmp::Capabilities capabilities() const;

private:
    //
    // Convenience structure to hold all the config parameters.
    // All units are S.I.
    //
    class Config 
    {
    public:
        Config();
    
        std::string host;
        int         port;
    
        // returns 0 if everything looks good.
        int verify( std::string& warnings, std::string& errors );

        void fromProperties( const hydroutil::Properties& props );
        std::string toString();
    };

    virtual void disable();

    bool enabled_;
    PlayerCc::PlayerClient    *robot_;
    PlayerCc::Position2dProxy *position2dProxy_;
    PlayerCc::Position3dProxy *position3dProxy_;
    PlayerCc::PowerProxy      *powerProxy_;

    // configuration
    Config config_;
    hydroutil::Context context_;
};

// Used for dynamically loading driver
class Factory : public hydrointerfaces::SegwayRmpFactory
{
public:
    hydrointerfaces::SegwayRmp *createDriver( const hydroutil::Context &context ) const
    {
        return new Driver( context );
    }
};

} // namespace

// Used for dynamically loading driver
extern "C" {
    hydrointerfaces::SegwayRmpFactory *createDriverFactory();
}

#endif
