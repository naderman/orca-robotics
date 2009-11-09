/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef HYDRO_ROBOT2D_PLAYER_CLIENT_H
#define HYDRO_ROBOT2D_PLAYER_CLIENT_H

#include <hydrointerfaces/robot2d.h>

// Player proxies
namespace PlayerCc
{
class PlayerClient;
class Position2dProxy;
class PowerProxy;
}

namespace robot2dplayerclient
{

class Driver : public hydrointerfaces::Robot2d
{
public:

    Driver( const hydroutil::Context& context );
    virtual ~Driver();

    // from Robot2d
    virtual void enable();
    virtual bool read( Data& data );
    virtual void write( const Command& command );
    virtual void getStatus( std::string &status, bool &isWarn, bool &isFault );
    virtual void applyHardwareLimits( double &maxForwardSpeed, double &maxReverseSpeed, 
                              double &maxTurnrate, double &maxTurnrateAtMaxSpeed );

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
        int port;
    
        // returns 0 if everything looks good.
        int verify( std::string& warnings, std::string& errors );

        void fromProperties( const hydroutil::Properties& props );
        std::string toString();
    };

    virtual void disable();

    bool enabled_;
    PlayerCc::PlayerClient    *robot_;
    PlayerCc::Position2dProxy *positionProxy_;
    PlayerCc::PowerProxy      *powerProxy_;

    // configuration
    Config config_;
    bool usePlayerTimestamp_;
    
    hydroutil::Context context_;
};

// Used for dynamically loading driver
class Factory : public hydrointerfaces::Robot2dFactory
{
public:
    hydrointerfaces::Robot2d *createDriver( const hydroutil::Context &context ) const
    {
        return new Driver( context );
    }
};

} // namespace

// Used for dynamically loading driver
extern "C" {
    hydrointerfaces::Robot2dFactory *createRobot2dDriverFactory();
}

#endif
