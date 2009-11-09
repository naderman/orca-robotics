/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#include <iostream>
#include <cmath>
#include <orcaice/orcaice.h>
#include <gbxutilacfr/mathdefs.h>

#include "vfhdriver.h"
#include "vfh_algorithm.h"
#include "vfh_algorithmconfig.h"

// using namespace localnav;
using namespace std;

namespace vfh {

// need definition (in case its not inlined)
const double VfhDriver::escapeTimeMs_;
	
VfhDriver::VfhDriver( const orcaice::Context                &context,
                      const orca::VehicleDescription        &descr,
                      const orca::RangeScanner2dDescription &scannerDescr )
    : stallRatio_(0.0),
      currentState_(STATE_GOAL_REACHED),
      heartbeater_(context),
      scannerDescr_(scannerDescr),
      context_(context)
{
    // Configure and instantiate the core vfh algorithm
    readFromProperties( context, vfhConfig_ );
    setFromVehicleDescr( descr, vfhConfig_ );

    cout<<"TRACE(vfhdriver.cpp): Instantiating VFH with: " << vfhConfig_ << endl;
    std::string warnings, errors;
    if ( vfhConfig_.checkSanity(warnings,errors) != 0 )
    {
        if ( warnings != "" )
        {
            context_.tracer().warning( "Dodgy-looking VFH configuration: " + warnings );
        }
        if ( errors != "" )
        {
            context_.tracer().error( "Erroneous VFH configuration: " + errors );
            stringstream ss;
            ss << "VfhDriver: Bad VFH config: " << errors;
            throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
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
        context_.tracer().debug( ss.str(), 2 );
        maxSpeed_ = vfhConfig_.maxSpeed;
    }
    if ( maxTurnrate_ > vfhConfig_.maxTurnrate1ms )
    {
        stringstream ss;
        ss << "VFH: requested maxTurnrate ("<<maxTurnrate*180.0/M_PI
           <<"deg) faster than its configured maximum ("<<vfhConfig_.absoluteMaxTurnrate*180.0/M_PI
           <<"deg).  Thresholding.";
        context_.tracer().debug( ss.str(), 2 );
        maxTurnrate_ = vfhConfig_.absoluteMaxTurnrate;
    }

    vfhAlgorithm_->SetCurrentMaxSpeed( (int) (maxSpeed_*1000.0) );
    vfhAlgorithm_->SetTurnrateThreshold( (int) (maxTurnrate_*180.0/M_PI) );
}

// Goal location is in robot's coordinate frame
hydronavutil::Velocity
VfhDriver::getCommand( const IDriver::Inputs &inputs )
{
    if ( inputs.goals.size() == 0 )
    {
        currentState_ = STATE_GOAL_REACHED;
        return hydronavutil::Velocity(0,0);
    }

    // Just deal with the current goal
    const orcalocalnav::Goal &goal = inputs.goals[0];

    maybeSendHeartbeat();
    setSpeedConstraints( goal.maxSpeed, goal.maxTurnrate );

    //
    // Four distinct cases
    //
    hydronavutil::Velocity cmd;
    if ( goalPosReached( goal ) )
    {
        // Stop us
        cmd = hydronavutil::Velocity(0,0);
        currentState_ = STATE_GOAL_REACHED;
    }
    else if ( shouldEscape( inputs.stalled ) )
    {
        // VFH isn't winning...  Have to try some escape manouvres
        cmd = escapeCommand( inputs.obsRanges );
        currentState_ = STATE_ESCAPING;
    }
    else if ( translationalGoalPosReached( goal ) )
    {
        // Turn in place
        cmd = turnToGoalCommand( goal );
        currentState_ = STATE_TURNING_AT_GOAL;
    }
    else
    {
        // Head for the goal
        cmd = approachGoalCommand( goal, inputs.currentVelocity, inputs.obsRanges );
        currentState_ = STATE_MOVING_TO_GOAL;
    }

    stringstream ss;
    ss << "VFH: Setting command: " << cmd;
    context_.tracer().debug( ss.str(), 5 );

    prevCmd_ = cmd;
    return cmd;
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
    if ( currentState_ == STATE_ESCAPING &&
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

hydronavutil::Velocity
VfhDriver::escapeCommand( const std::vector<float> &obsRanges )
{
    if ( currentState_ != STATE_ESCAPING ||
         escapeTimer_.elapsedMs() > escapeTimeMs_ )
    {
        // Choose a new random escape command
        float escapeSpeed       = ((2.0*(float)rand()/(float)RAND_MAX)-1.0) * maxSpeed_;
        float maxEscapeTurnrate = vfhAlgorithm_->GetMaxTurnrate((int)(escapeSpeed*1000.0)) * M_PI/180.0;
        float escapeTurnrate    = ((2.0*(float)rand()/(float)RAND_MAX)-1.0) * maxEscapeTurnrate;

        cout<<"TRACE(vfhdriver.cpp): escapeSpeed: " << escapeSpeed << endl;
        cout<<"TRACE(vfhdriver.cpp): max is: " << maxSpeed_ << endl;
        escapeTimer_.restart();
        return hydronavutil::Velocity( escapeSpeed, escapeTurnrate );
    }
    else
    {
        // Continue the escape
        return prevCmd_;
    }
}

hydronavutil::Velocity
VfhDriver::turnToGoalCommand( const orcalocalnav::Goal &goal )
{
    hydronavutil::Velocity cmd(0,0);

    float posNeg = 1.0;
    if ( goal.theta < 0.0 ) posNeg = -1.0;

    if ( currentState_ != STATE_TURNING_AT_GOAL )
    {
        // Just got here: turn hard.
        cmd.rot() = posNeg * maxTurnrate_;
    }
    else
    {
        // We're already turning.  Keep doing so.
        float maxAllowedTurnrate = fabs(prevCmd_.rot());

        // Check for overshoot
        float prevPosNeg = 1.0;
        if ( prevCmd_.rot() < 0.0 ) prevPosNeg = -1.0;

        if ( posNeg != prevPosNeg )
        {
            // Overshoot detected: slow down.
            maxAllowedTurnrate = maxAllowedTurnrate * 0.9;
        }

        cmd.rot() = posNeg*maxAllowedTurnrate;
    }
    return cmd;
}

void 
VfhDriver::copyLaserScan( const std::vector<float> &obsRanges, double playerLaserScan[401][2] )
{
    double angleIncrement = scannerDescr_.fieldOfView / double(scannerDescr_.numberOfSamples-1);
    const float EPS = 1e-9;

    // number of times to replicate each scan when copying
    int replicateNum=1;
    if ( obsRanges.size() == 181 )
    {
        if ( !NEAR(angleIncrement, 1.0*M_PI/180.0, EPS) )
        {
            stringstream ss;
            ss << "VfhDriver: Can't handle weird angle increment: obs size,increment= " << obsRanges.size() << ", " << angleIncrement*180.0/M_PI << "deg";
            throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
        }
        replicateNum = 2;
    }
    else if ( obsRanges.size() == 361 )
    {
        if ( !NEAR(angleIncrement, 0.5*M_PI/180.0, EPS) )
        {
            stringstream ss;
            ss << "VfhDriver: Can't handle weird angle increment: obs size,increment= " << obsRanges.size() << ", " << angleIncrement*180.0/M_PI << "deg";
            throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
        }
        replicateNum = 1;
    }
    else
    {
//         if ( !NEAR(angleIncrement, 0.5*M_PI/180.0, EPS) )
//         {
            stringstream ss;
            ss << "VfhDriver: obs size,increment= " << obsRanges.size() << ", " << angleIncrement*180.0/M_PI << endl;
            ss << "Expected " << 0.5*M_PI/180.0 << ", found " << angleIncrement;
            context_.tracer().debug( ss.str(), 5 );
            ss.str("");
            ss << "VfhDriver: Can't handle weird angle increment: obs size,increment= " << obsRanges.size() << ", " << angleIncrement*180.0/M_PI << "deg";
            throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
//         }
//         // map the values in the range array to degrees so that they can be copied into the
//         // player structure at the end of this function
//         // i.e ranges[i] and ranges [i+1] is now set to the range for angle i
//         double currentAngle = 0.0;
//         for ( int i=0; i < (int) obsRanges.size(); i++ )
//         {
//             obsRanges[(int) floor(currentAngle)] = obsRanges[i] ;
//             currentAngle = currentAngle + angleIncrement;
//         }
//         replicateNum = 1;
    }
    if ( !NEAR(scannerDescr_.startAngle, -90.0*M_PI/180.0, EPS) )
    {
        stringstream ss;
        ss << "VfhDriver: Don't know how to handle weird startAngle: " << scannerDescr_.startAngle;
        throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
    }

    // Copy the ranges into a player-style structure.  This means converting units: m -> mm.
    int j = 0; // references into orca structure
    int c = 0;
    for ( int i=0; i < 361; i++ )
    {
        playerLaserScan[i][0] = obsRanges[j]*1000;
        if ( ++c == replicateNum )
        {
            j++;
            c=0;
        }
    }
}

hydronavutil::Velocity
VfhDriver::approachGoalCommand( const orcalocalnav::Goal     &goal, 
                                const hydronavutil::Velocity &currentVelocity,
                                const std::vector<float>     &obsRanges )
{
    // Copy stuff into the format required by vfh_algorithm

    // speed in mm/s
    int currentSpeed    = (int) (currentVelocity.lin() * 1000.0);

    // goalDirection in [0,360), where 90 is straight ahead
    float goalDirection = (directionToGoal(goal)*180/M_PI)+90.0;
    if ( goalDirection < 0.0 ) goalDirection += 360.0;

    // distance in mm
    float goalDistance = distanceToGoal(goal) * 1000.0;

    // tolerance in mm
    float goalDistanceTolerance = goal.distanceTolerance * 1000.0;

    copyLaserScan( obsRanges, playerLaserScan_ );

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
    return hydronavutil::Velocity( (float)chosenSpeed/1000.0, chosenTurnrate*M_PI/180.0 );
}

void
VfhDriver::maybeSendHeartbeat()
{
    if ( !heartbeater_.isHeartbeatTime() )
        return;

    // construct the heartbeat message
    stringstream ss;
    ss << "VfhDriver: Current state: " << currentState_;
    heartbeater_.beat( ss.str() );
}

std::ostream &operator<<( std::ostream &s, VfhDriver::DriverState state )
{
    switch (state)
    {
    case VfhDriver::STATE_GOAL_REACHED:
    {
        s << "GOAL_REACHED";
        break;
    }
    case VfhDriver::STATE_TURNING_AT_GOAL:
    {
        s << "TURNING_AT_GOAL";
        break;
    }
    case VfhDriver::STATE_ESCAPING:
    {
        s << "ESCAPING";
        break;
    }
    case VfhDriver::STATE_MOVING_TO_GOAL:
    {
        s << "MOVING_TO_GOAL";
        break;
    }
    default:
    {
        s << "Error: Unknown state";
        break;
    }        
    }
    return s;
}

}

extern "C" {
    orcalocalnav::DriverFactory *createLocalNavDriverFactory()
    { return new vfh::VfhDriverFactory; }
}
