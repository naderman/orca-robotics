#include "vehiclesimulator.h"
#include <iostream>
#include <hydropathplan/util.h>
#include <sstream>

using namespace std;

namespace hydrosim2d {

namespace {

    bool
    bordersFreeSpace( const hydroogmap::OgMap &ogMap, const hydroogmap::GridIndices &gridIndices )
    {
        return ( ogMap.gridCell( gridIndices.x-1, gridIndices.y ) < hydroogmap::CELL_UNKNOWN ||
                 ogMap.gridCell( gridIndices.x+1, gridIndices.y ) < hydroogmap::CELL_UNKNOWN ||
                 ogMap.gridCell( gridIndices.x,   gridIndices.y-1 ) < hydroogmap::CELL_UNKNOWN ||
                 ogMap.gridCell( gridIndices.x,   gridIndices.y+1 ) < hydroogmap::CELL_UNKNOWN );
    }

}

////////////////////////////////////////////////////////////////////////////////

VehicleSimulator::VehicleSimulator( const Config            &config,
                                    const hydroogmap::OgMap &ogMap,
                                    IPosePublisher          &posePublisher,
                                    IOdomPublisher          *odomPublisher )
    : config_(config),
      grownOgMap_(ogMap),
      pose_(0,0,0),
      velocity_(0,0),
      posePublisher_(posePublisher),
      odomPublisher_(odomPublisher)
{
    const double traversabilityThreshhold = 0.5;
    hydropathplan::util::growObstaclesOgMap( grownOgMap_,
                                             traversabilityThreshhold,
                                             (int)(config_.robotRadius/grownOgMap_.metresPerCell()) );
}

void 
VehicleSimulator::act( const hydronavutil::Velocity &cmd )
{
    // Check acceleration
    if ( config_.checkDifferentialConstraints )
    {
        double accLin = (velocity_.lin() - cmd.lin()) / config_.timeStep;
        double accRot = (velocity_.rot() - cmd.rot()) / config_.timeStep;

        bool linAccExceeded = (fabs(accLin)-config_.maxLinearAcceleration) > 1e-3;
        bool rotAccExceeded = (fabs(accRot)-config_.maxRotationalAcceleration) > 1e-3;
        if ( linAccExceeded || rotAccExceeded )
        {
            stringstream ss;
            ss << "Acceleration exceeds maximum ";
            if ( linAccExceeded )
                ss << "(lin too big)";
            if ( rotAccExceeded )
                ss << "(rot too big)";
            ss << endl;
            ss << "  vel:     " << velocity_ << endl
               << "  cmd:     " << cmd << endl
               << "  =acc:    " << accLin << ", " << accRot*180.0/M_PI << "deg" << endl
               << "  max acc: " << config_.maxLinearAcceleration << ", " << config_.maxRotationalAcceleration*180.0/M_PI << "deg";
            throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
        }
    }

    // check lateral acceleration
    if ( config_.checkLateralAcceleration )
    {
        const double cmdLateralAcceleration = cmd.lin()*cmd.rot();
        if (  (cmdLateralAcceleration-config_.maxLateralAcceleration) > 1e-3 )
        {
            stringstream ss;
            ss << "exceeded lateral acceleration limit:" << endl
               << "Limit       : " << config_.maxLateralAcceleration << endl
               << "Command     : " << cmd << endl
               << "Lateral Acc : " << cmdLateralAcceleration << endl;
            throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
        }
    }

    // Check velocity constraints
    if ( config_.checkVelocityConstraints )
    {
        if ( cmd.lin() < config_.minVelocity.lin() ||
             cmd.rot() < config_.minVelocity.rot() ||
             cmd.lin() > config_.maxVelocity.lin() ||
             cmd.rot() > config_.maxVelocity.rot() )
        {
            stringstream ss;
            ss << "Command " << cmd << " exceeds velocity limits of min=" << config_.minVelocity << ", max=" << config_.maxVelocity;
            throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );        
        }
    }
    
    // Set it
    velocity_ = cmd;

    // Then apply it
    const bool normaliseHeading=true;
    hydronavutil::addPoseOffset( pose_.x(),
                                 pose_.y(),
                                 pose_.theta(),
                                 velocity_.lin()*config_.timeStep,
                                 0,
                                 velocity_.rot()*config_.timeStep,
                                 normaliseHeading );

    posePublisher_.publish( pose_ );
    if ( odomPublisher_ )
        odomPublisher_->publish( pose_, velocity_ );
}

bool 
VehicleSimulator::isCollided() const
{
    assert( grownOgMap_.coordsWithinMap( pose_.x(), pose_.y() ) && "Vehicle outside map!" );

    if ( grownOgMap_.worldCell( pose_.x(), pose_.y() ) > hydroogmap::CELL_UNKNOWN )
    {
        // Cell is occupied.  To be really sure, make sure we're _inside_ an obstacle
        // (discrectisation can screw with things, so give the controller the benefit of the doubt).
        if ( !bordersFreeSpace( grownOgMap_, grownOgMap_.gridIndices( pose_.x(), pose_.y() ) ) )
        {
            return true;
        }
        else
        {
            cout<<"TRACE(vehiclesimulator.cpp): Warning: touched an edge." << endl;
        }
    }
    return false;
}



}

