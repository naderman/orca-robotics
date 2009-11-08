/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef HYDRO_BICYCLE_PLAYER_CLIENT_H
#define HYDRO_BICYCLE_PLAYER_CLIENT_H

#include <hydrointerfaces/bicycle.h>
#include <memory>

// Player proxies
namespace PlayerCc
{
class PlayerClient;
class Position2dProxy;
class Position1dProxy;
}

namespace bicycleplayerclient
{

class Driver : public hydrointerfaces::Bicycle
{
public:

    Driver( const hydroutil::Context& context );
    virtual ~Driver();

    // from Bicycle
    virtual void enable();
    virtual bool read( Data& data );
    virtual void write( const Command& command );
    virtual void getStatus( std::string &status, bool &isWarn, bool &isFault );

private:
    //
    // Convenience structure to hold all the config parameters.
    // All units are S.I.
    //
    class Config 
    {
    public:
        Config();
    
        const char *host;
        int port;
	bool usePlayerTimestamp;
        int position2dIndex;
        int position1dIndex; // -1 if not used
        
        // the following parameter is used when position1dIndex = -1

	// if set to non-zero, the driver simulates a maximum steer angle rate
	// in the commands sent to Player, which is useful under Stage, given
	// that carlike models repond immediately to steer angle commands
        double simulatedMaxSteerAngleRate;
    
        void fromProperties( const hydroutil::Properties& props );
        std::string toString();
    };

    virtual void disable();

    bool enabled_;
    std::auto_ptr<PlayerCc::PlayerClient> robot_;
    std::auto_ptr<PlayerCc::Position2dProxy> positionProxy_;
    
    // additional proxy to obtain current steer angle from Player (optional)
    std::auto_ptr<PlayerCc::Position1dProxy> currentSteerAngleProxy_;

    // if no proxy is available to obtain real steer angles, just simulate it
    double simulatedCurrentSteerAngle_;
    double simulatedCurrentSteerAngleTimestamp_; // in seconds
    double simulatedTargetSteerAngle_;

    // configuration
    Config config_;
    
    hydroutil::Context context_;
};

// Used for dynamically loading driver
class Factory : public hydrointerfaces::BicycleFactory
{
public:
    hydrointerfaces::Bicycle *createDriver( const hydroutil::Context &context ) const
    {
        return new Driver( context );
    }
};

} // namespace

// Used for dynamically loading driver
extern "C" {
    hydrointerfaces::BicycleFactory *createBicycleDriverFactory();
}

#endif
