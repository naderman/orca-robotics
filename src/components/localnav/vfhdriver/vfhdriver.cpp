/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#include <iostream>
#include <cmath>
#include <orcaice/orcaice.h>

#include "vfhdriver.h"
#include "vfh_algorithm.h"
#include "vfh_algorithmconfig.h"

using namespace localnav;
using namespace std;

namespace vfh {

// need definition (in case its not inlined)
const double VfhDriver::escapeTimeMs_;
	
VfhDriver::VfhDriver( const orcaice::Context & context )
    : stallRatio_(0.0),
      currentState_(IDriver::STATE_GOAL_REACHED),
      context_(context)
{
    // Configure and instantiate the core vfh algorithm
    readFromProperties( context, vfhConfig_ );
    cout<<"TRACE(vfhdriver.cpp): Instantiating VFH with: " << vfhConfig_ << endl;
    std::string warnings, errors;
    if ( vfhConfig_.checkSanity(warnings,errors) != 0 )
    {
        if ( warnings != "" )
        {
            context_.tracer()->warning( "Dodgy-looking VFH configuration: " + warnings );
        }
        if ( errors != "" )
        {
            context_.tracer()->error( "Erroneous VFH configuration: " + errors );
            throw( std::string( "Bad VFH config" ) );
        }
    }

    vfhAlgorithm_ = new VFH_Algorithm( vfhConfig_ );
}

VfhDriver::~VfhDriver()
{
    if ( vfhAlgorithm_ ) delete vfhAlgorithm_;
}

void 
VfhDriver::setSpeedConstraints( float maxSpeed, float maxTurnrate )
{
    maxSpeed_ = maxSpeed;
    maxTurnrate_ = maxTurnrate;

    // Check that this doesn't exceed any of our hard-coded maximums
    if ( maxSpeed_ > vfhConfig_.maxSpeed )
    {
        stringstream ss;
        ss << "VFH: requested maxSpeed ("
           <<maxSpeed_<<") faster than its configured maximum ("<<vfhConfig_.maxSpeed
           <<").  Thresholding.";
        context_.tracer()->debug( ss.str(), 1 );
        maxSpeed_ = vfhConfig_.maxSpeed;
    }
    if ( maxTurnrate_ > vfhConfig_.maxTurnrate1ms )
    {
        stringstream ss;
        ss << "VFH: requested maxTurnrate ("<<maxTurnrate*180.0/M_PI
           <<"deg) faster than its configured maximum ("<<vfhConfig_.absoluteMaxTurnrate*180.0/M_PI
           <<"deg).  Thresholding.";
        context_.tracer()->debug( ss.str(), 1 );
        maxTurnrate_ = vfhConfig_.absoluteMaxTurnrate;
    }

    vfhAlgorithm_->SetCurrentMaxSpeed( (int) (maxSpeed_*1000.0) );
    vfhAlgorithm_->SetTurnrateThreshold( (int) (maxTurnrate_*180.0/M_PI) );
}

// Goal location is in robot's coordinate frame
IDriver::DriverState
VfhDriver::getCommand( bool  stalled,
                       const orca::Twist2d &currentVelocity,
                       const orca::RangeScanner2dDataPtr obs,
                       const Goal                        &goal,
                       orca::VelocityControl2dData& cmd )
{
    setSpeedConstraints( goal.maxSpeed, goal.maxTurnrate );

    //
    // Four distinct cases
    //
    if ( goalPosReached( goal ) )
    {
        // Stop us
        setToZero( cmd );
        currentState_ = IDriver::STATE_GOAL_REACHED;
    }
    else if ( shouldEscape( stalled ) )
    {
        // VFH isn't winning...  Have to try some escape manouvres
        setToEscape( cmd, obs );
        currentState_ = IDriver::STATE_ESCAPING;
    }
    else if ( translationalGoalPosReached( goal ) )
    {
        // Turn in place
        setTurnToGoal( cmd, goal );
        currentState_ = IDriver::STATE_TURNING_AT_GOAL;
    }
    else
    {
        // Head for the goal
        setToApproachGoal( cmd, goal, currentVelocity, obs );
        currentState_ = IDriver::STATE_MOVING_TO_GOAL;
    }

    stringstream ss;
    ss << "VFH: Setting command: " << orcaice::toString(cmd);
    context_.tracer()->debug( ss.str(), 5 );

    prevCmd_.motion.v.x = cmd.motion.v.x;
    prevCmd_.motion.v.y = cmd.motion.v.y;
    prevCmd_.motion.w   = cmd.motion.w;

    return currentState_;
}

bool
VfhDriver::shouldEscape( bool stalled )
{
    // Fix up stall ratio
    if ( stalled )
    {
        stallRatio_ = 0.90*stallRatio_ + 0.1;
    }
    else
    {
        stallRatio_=0.90*stallRatio_;
    }

    // If we're in the middle of an escape, continue
    // Otherwise check the stall ratio.
    if ( currentState_ == IDriver::STATE_ESCAPING &&
         escapeTimer_.elapsedMs() < escapeTimeMs_ )
    {
        return true;
    }
    else if ( stallRatio_ >= 0.4 )
    {
        return true;
    }
    return false;
}

void
VfhDriver::setToZero( orca::VelocityControl2dData& cmd )
{
    cmd.motion.v.x = 0.0;
    cmd.motion.v.y = 0.0;
    cmd.motion.w   = 0.0;
}

void
VfhDriver::setToEscape( orca::VelocityControl2dData& cmd, const orca::RangeScanner2dDataPtr &obs )
{
    if ( currentState_ != IDriver::STATE_ESCAPING ||
         escapeTimer_.elapsedMs() > escapeTimeMs_ )
    {
        // Choose a new random escape command
        float escapeSpeed       = ((2.0*(float)rand()/(float)RAND_MAX)-1.0) * maxSpeed_;
        float maxEscapeTurnrate = vfhAlgorithm_->GetMaxTurnrate((int)(cmd.motion.v.x*1000.0)) * M_PI/180.0;
        float escapeTurnrate    = ((2.0*(float)rand()/(float)RAND_MAX)-1.0) * maxEscapeTurnrate;

        cout<<"TRACE(vfhdriver.cpp): escapeSpeed: " << escapeSpeed << endl;
        cout<<"TRACE(vfhdriver.cpp): max is: " << maxSpeed_ << endl;
        cmd.motion.v.x = escapeSpeed;
        cmd.motion.v.y = 0.0;
        cmd.motion.w   = escapeTurnrate;

        escapeTimer_.restart();
    }
    else
    {
        // Continue the escape
        cmd.motion.v.x = prevCmd_.motion.v.x;
        cmd.motion.v.y = prevCmd_.motion.v.y;
        cmd.motion.w   = prevCmd_.motion.w;
    }
}

void
VfhDriver::setTurnToGoal( orca::VelocityControl2dData& cmd, const Goal &goal )
{
    cmd.motion.v.x = 0.0;
    cmd.motion.v.y = 0.0;

    float posNeg = 1.0;
    if ( goal.theta < 0.0 ) posNeg = -1.0;

    if ( currentState_ != IDriver::STATE_TURNING_AT_GOAL )
    {
        // Just got here: turn hard.
        cmd.motion.w = posNeg * maxTurnrate_;
    }
    else
    {
        // We're already turning.  Keep doing so.
        float maxAllowedTurnrate = fabs(prevCmd_.motion.w);

        // Check for overshoot
        float prevPosNeg = 1.0;
        if ( prevCmd_.motion.w < 0.0 ) prevPosNeg = -1.0;

        if ( posNeg != prevPosNeg )
        {
            // Overshoot detected: slow down.
            maxAllowedTurnrate = maxAllowedTurnrate * 0.9;
        }

        cmd.motion.w = posNeg*maxAllowedTurnrate;
    }
}

void 
VfhDriver::copyLaserScan( const orca::RangeScanner2dDataPtr obs, double playerLaserScan[401][2] )
{
    double angleIncrement = obs->fieldOfView / double(obs->ranges.size()+1);
    const float EPS = 1e-9;

    // number of times to replicate each scan when copying
    int replicateNum;
    if ( obs->ranges.size() == 181 )
    {
        if ( angleIncrement - 1.0*M_PI/180.0 > EPS ) 
        {
            cout<<"TRACE(vfhdriver.cpp): obs size,increment= " << obs->ranges.size() << ", " << angleIncrement*180.0/M_PI << endl;
            context_.tracer()->error( "VfhDriver: Can't handle weird angle increment." );
            throw( std::string("VfhDriver: Can't handle weird angle increment.") );
        }
        replicateNum = 2;
    }
    else if ( obs->ranges.size() == 361 )
    {
        if ( angleIncrement - 0.5*M_PI/180.0 > EPS ) 
        {
            cout<<"TRACE(vfhdriver.cpp): obs size,increment= " << obs->ranges.size() << ", " << angleIncrement*180.0/M_PI << endl;
            cout<<"TRACE(vfhdriver.cpp): expected " << 0.5*M_PI/180.0 << ", found " << angleIncrement << endl;
            printf("%.20f, %.20f\n",0.5*M_PI/180.0,angleIncrement);

            context_.tracer()->error( "VfhDriver: Can't handle weird angle increment." );
            throw( std::string("VfhDriver: Can't handle weird angle increment.") );
        }
        replicateNum = 1;
    }
    else
    {
        throw( std::string("VfhDriver: This scan size is not implemented.") );
    }
    if ( obs->startAngle - -90.0*M_PI/180.0 > EPS )
    {
        throw( std::string("VfhDriver: Don't know how to handle weird startAngle.") );
    }

    // Copy the ranges into a player-style structure.  This means converting units: m -> mm.
    int j = 0; // references into orca structure
    int c = 0;
    for ( int i=0; i < 361; i++ )
    {
        playerLaserScan[i][0] = obs->ranges[j]*1000;
        if ( ++c == replicateNum )
        {
            j++;
            c=0;
        }
    }
}

void
VfhDriver::setToApproachGoal( orca::VelocityControl2dData& cmd,
                              const Goal &goal, 
                              const orca::Twist2d &currentVelocity,
                              const orca::RangeScanner2dDataPtr &obs )
{
    // Copy stuff into the format required by vfh_algorithm

    // speed in mm/s
    int currentSpeed    = (int) (currentVelocity.v.x * 1000.0);

    // goalDirection in [0,360), where 90 is straight ahead
    float goalDirection = (directionToGoal(goal)*180/M_PI)+90.0;
    if ( goalDirection < 0.0 ) goalDirection += 360.0;

    // distance in mm
    float goalDistance = distanceToGoal(goal) * 1000.0;

    // tolerance in mm
    float goalDistanceTolerance = goal.distanceTolerance * 1000.0;

    copyLaserScan( obs, playerLaserScan_ );

//     cout<<"TRACE(vfhdriver.cpp): calling to algorithm, with:" << endl;
//     cout<<"TRACE(vfhdriver.cpp):   goalDirection: " << goalDirection << endl;
//     cout<<"TRACE(vfhdriver.cpp):   goalDistance:  " << goalDistance << endl;

    // Get VFH's choice
    int chosenSpeed, chosenTurnrate;
    vfhAlgorithm_->Update_VFH( playerLaserScan_,
                               currentSpeed,
                               goalDirection,
                               goalDistance,
                               goalDistanceTolerance,
                               chosenSpeed,
                               chosenTurnrate );

    // Now copy back from player format
    cmd.motion.v.x = (float)chosenSpeed/1000.0;
    cmd.motion.v.y = 0.0;
    cmd.motion.w   = chosenTurnrate*M_PI/180.0;
}

}
