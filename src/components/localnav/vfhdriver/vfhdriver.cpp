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

VfhDriver::VfhDriver( const GoalWatcher        &goalWatcher,
                      orcaice::Context          context )
    : LocalNavDriver(goalWatcher),
      stallRatio_(0.0),
      currentState_(LocalNavDriver::STATE_GOAL_REACHED),
      prevCmd_(new orca::Velocity2dCommand),
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
VfhDriver::setLocalNavParameters( LocalNavParameters params )
{
    stringstream ssParams;
    ssParams << "VFH: Setting params: " << params;
    context_.tracer()->debug( ssParams.str(), 5 );

    localNavParameters_ = params;

    // Check that this doesn't exceed any of our hard-coded maximums
    if ( params.maxSpeed > vfhConfig_.maxSpeed )
    {
        localNavParameters_.maxSpeed = vfhConfig_.maxSpeed;

        stringstream ss;
        ss << "VFH: requested maxSpeed ("
           <<params.maxSpeed<<") faster than its configured maximum ("<<vfhConfig_.maxSpeed
           <<").  Thresholding.";
        context_.tracer()->warning( ss.str() );
    }
    if ( params.maxTurnrate > vfhConfig_.maxTurnrate1ms )
    {
        localNavParameters_.maxTurnrate = vfhConfig_.absoluteMaxTurnrate;

        stringstream ss;
        ss << "VFH: requested maxTurnrate ("<<params.maxTurnrate
           <<") faster than its configured maximum ("<<vfhConfig_.absoluteMaxTurnrate
           <<").  Thresholding.";
        context_.tracer()->warning( ss.str() );
    }

    assert( localNavParameters_.maxSpeed >= 0.0 );
    assert( localNavParameters_.maxTurnrate >= 0.0 );

    vfhAlgorithm_->SetCurrentMaxSpeed( (int) (localNavParameters_.maxSpeed*1000.0) );
    vfhAlgorithm_->SetTurnrateThreshold( (int) (localNavParameters_.maxTurnrate*180.0/M_PI) );
}

// Goal location is in robot's coordinate frame
LocalNavDriver::DriverState
VfhDriver::getCommand( bool  stalled,
                       const orca::Twist2d &currentVelocity,
                       const orca::RangeScannerDataPtr obs,
                       orca::Velocity2dCommandPtr &cmd )
{
    //
    // Four distinct cases
    //
    if ( goalWatcher_.goalReached() )
    {
        // Stop us
        setToZero( cmd );
        currentState_ = LocalNavDriver::STATE_GOAL_REACHED;
    }
    else if ( shouldEscape( stalled ) )
    {
        // VFH isn't winning...  Have to try some escape manouvres
        setToEscape( cmd, obs );
        currentState_ = LocalNavDriver::STATE_ESCAPING;
    }
    else if ( goalWatcher_.translationalGoalReached() )
    {
        // Turn in place
        setTurnToGoal( cmd, goalWatcher_ );
        currentState_ = LocalNavDriver::STATE_TURNING_AT_GOAL;
    }
    else
    {
        // Head for the goal
        setToApproachGoal( cmd, goalWatcher_, currentVelocity, obs );
        currentState_ = LocalNavDriver::STATE_MOVING_TO_GOAL;
    }

    prevCmd_->motion.v.x = cmd->motion.v.x;
    prevCmd_->motion.v.y = cmd->motion.v.y;
    prevCmd_->motion.w   = cmd->motion.w;

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
    if ( currentState_ == LocalNavDriver::STATE_ESCAPING &&
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
VfhDriver::setToZero( orca::Velocity2dCommandPtr &cmd )
{
    cmd->motion.v.x = 0.0;
    cmd->motion.v.y = 0.0;
    cmd->motion.w   = 0.0;
}

void
VfhDriver::setToEscape( orca::Velocity2dCommandPtr &cmd, const orca::RangeScannerDataPtr &obs )
{
    if ( currentState_ != LocalNavDriver::STATE_ESCAPING ||
         escapeTimer_.elapsedMs() > escapeTimeMs_ )
    {
        // Choose a new random escape command
        float escapeSpeed       = ((2.0*(float)rand()/(float)RAND_MAX)-1.0) * localNavParameters_.maxSpeed;
        float maxEscapeTurnrate = vfhAlgorithm_->GetMaxTurnrate((int)(cmd->motion.v.x*1000.0)) * M_PI/180.0;
        float escapeTurnrate    = ((2.0*(float)rand()/(float)RAND_MAX)-1.0) * maxEscapeTurnrate;

        cout<<"TRACE(vfhdriver.cpp): escapeSpeed: " << escapeSpeed << endl;
        cout<<"TRACE(vfhdriver.cpp): max is: " << localNavParameters_.maxSpeed << endl;
        cmd->motion.v.x = escapeSpeed;
        cmd->motion.v.y = 0.0;
        cmd->motion.w   = escapeTurnrate;

        escapeTimer_.restart();
    }
    else
    {
        // Continue the escape
        cmd->motion.v.x = prevCmd_->motion.v.x;
        cmd->motion.v.y = prevCmd_->motion.v.y;
        cmd->motion.w   = prevCmd_->motion.w;
    }
}

void
VfhDriver::setTurnToGoal( orca::Velocity2dCommandPtr &cmd, const GoalWatcher &goalWatcher )
{
    cmd->motion.v.x = 0.0;
    cmd->motion.v.y = 0.0;

    float posNeg = 1.0;
    if ( goalWatcher.headingDifference() < 0.0 ) posNeg = -1.0;

    if ( currentState_ != LocalNavDriver::STATE_TURNING_AT_GOAL )
    {
        // Just got here: turn hard.
        cmd->motion.w = posNeg * localNavParameters_.maxTurnrate;
    }
    else
    {
        // We're already turning.  Keep doing so.
        float maxAllowedTurnrate = prevCmd_->motion.w;

        // Check for overshoot
        float prevPosNeg = 1.0;
        if ( prevCmd_->motion.w < 0.0 ) prevPosNeg = -1.0;

        if ( posNeg != prevPosNeg )
        {
            // Overshoot detected: slow down.
            maxAllowedTurnrate = maxAllowedTurnrate / 2.0;
        }

        cmd->motion.w = maxAllowedTurnrate;
    }
}

void 
VfhDriver::copyLaserScan( const orca::RangeScannerDataPtr obs, double playerLaserScan[401][2] )
{
    const float EPS = 1e-9;

    // number of times to replicate each scan when copying
    int replicateNum;
    if ( obs->ranges.size() == 181 )
    {
        if ( obs->angleIncrement - 1.0*M_PI/180.0 > EPS ) 
        {
            cout<<"TRACE(vfhdriver.cpp): obs size,increment= " << obs->ranges.size() << ", " << obs->angleIncrement*180.0/M_PI << endl;
            context_.tracer()->error( "VfhDriver: Can't handle weird angle increment." );
            throw( std::string("VfhDriver: Can't handle weird angle increment.") );
        }
        replicateNum = 2;
    }
    else if ( obs->ranges.size() == 361 )
    {
        if ( obs->angleIncrement - 0.5*M_PI/180.0 > EPS ) 
        {
            cout<<"TRACE(vfhdriver.cpp): obs size,increment= " << obs->ranges.size() << ", " << obs->angleIncrement*180.0/M_PI << endl;
            cout<<"TRACE(vfhdriver.cpp): expected " << 0.5*M_PI/180.0 << ", found " << obs->angleIncrement << endl;
            printf("%.20f, %.20f\n",0.5*M_PI/180.0,obs->angleIncrement);

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
VfhDriver::setToApproachGoal( orca::Velocity2dCommandPtr &cmd,
                              const GoalWatcher &goalWatcher, 
                              const orca::Twist2d &currentVelocity,
                              const orca::RangeScannerDataPtr &obs )
{
    // Copy stuff into the format required by vfh_algorithm

    // speed in mm/s
    int currentSpeed    = (int) (currentVelocity.v.x * 1000.0);

    // goalDirection in [0,360), where 90 is straight ahead
    float goalDirection = (goalWatcher_.goalDirection()*180/M_PI)+90.0;
    if ( goalDirection < 0.0 ) goalDirection += 360.0;

    // distance in mm
    float goalDistance = goalWatcher_.goalDistance() * 1000.0;

    // tolerance in mm
    float goalDistanceTolerance = goalWatcher_.goalDistanceTolerance() * 1000.0;

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
    cmd->motion.v.x = (float)chosenSpeed/1000.0;
    cmd->motion.v.y = 0.0;
    cmd->motion.w   = chosenTurnrate*M_PI/180.0;
}

}
