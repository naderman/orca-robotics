#include "multiplicativenoisemodel.h"
#include <iostream>
#include <orcaice/orcaice.h>
#include <hydroutil/rand.h>

using namespace std;

namespace noisyrobot2d {

MultiplicativeNoiseModel::MultiplicativeNoiseModel( NoiseLevelDistributionType noiseLevelType, 
                                                    double                     noiseLevelLinear,
                                                    double                     noiseLevelRotational,
                                                    double                     constantMultiplierTime )
    : currentCommandVelScale_(1.0),
      currentCommandYawScale_(1.0),
      constantMultiplierTime_(constantMultiplierTime),
      noiseLevelType_(noiseLevelType),
      noiseLevelLinear_(noiseLevelLinear),
      noiseLevelRotational_(noiseLevelRotational)
{
    lastScaleFactorChangeTime_.seconds = 0;
    lastScaleFactorChangeTime_.useconds = 0;
}

void 
MultiplicativeNoiseModel::perturbCommand( orca::Velocity2dCommand& command )
{
    getNewScaleFactors();

    // cout<<"TRACE(multiplicativenoisemodel.cpp): instead of: " << orcaice::toString(command) << endl;

    command.motion.v.x *= currentCommandVelScale_;
    command.motion.w   *= currentCommandYawScale_;

    // cout<<"TRACE(multiplicativenoisemodel.cpp): apply:      " << orcaice::toString(command) << endl;
}

void 
MultiplicativeNoiseModel::perturbOdom( ::orca::Position2dData& odom )
{
    // Don't let the subscriber know that we're fucking with their commands:
    //   perturb the odometry inversely to how we perturbed the last command

    //
    // Work out the deltaPose
    //
    hydronavutil::Offset deltaPose;
    odomDifferentiator_.addOdom( odom, deltaPose );

    // cout<<"TRACE(multiplicativenoisemodel.cpp): instead of deltaPose="<<deltaPose << endl;

    //
    // Hide the warping of the deltaPose
    //
    deltaPose.x     /= currentCommandVelScale_;
    deltaPose.theta /= currentCommandYawScale_;

    // cout<<"TRACE(multiplicativenoisemodel.cpp): report     deltaPose="<<deltaPose << endl;

    //
    // Add the delta to apparentOdom_
    //
    hydronavutil::addPoseOffset( apparentOdom_.x,
                                apparentOdom_.y,
                                apparentOdom_.theta,
                                deltaPose.x,
                                deltaPose.y,
                                deltaPose.theta,
                                false );

    //
    // Report apparentOdom_ instead of the true odom
    //
    odom.pose.p.x = apparentOdom_.x;
    odom.pose.p.y = apparentOdom_.y;
    odom.pose.o   = apparentOdom_.theta;

    //
    // Hide the warping of the velocities
    //
    odom.motion.v.x /= currentCommandVelScale_;
    odom.motion.w   /= currentCommandYawScale_;
}

void 
MultiplicativeNoiseModel::getNewScaleFactors()
{
    if ( orcaice::timeDiffAsDouble( orcaice::getNow(), lastScaleFactorChangeTime_ ) > constantMultiplierTime_ )
    {
        if ( noiseLevelType_ == DISTRIBUTION_GAUSSIAN )
        {
            currentCommandVelScale_ = hydroutil::normalRand( 1.0, noiseLevelLinear_ );
            currentCommandYawScale_ = hydroutil::normalRand( 1.0, noiseLevelRotational_ );
        }
        else if ( noiseLevelType_ == DISTRIBUTION_UNIFORM )
        {
            cout<<"TRACE(multiplicativenoisemodel.cpp): choosing linear from range:" << endl;
            cout<<"TRACE(multiplicativenoisemodel.cpp): " << 1.0-noiseLevelLinear_<<", "<<1.0+noiseLevelLinear_<< endl;
            currentCommandVelScale_ = 
                hydroutil::randNumInclusive( 1.0-noiseLevelLinear_, 1.0+noiseLevelLinear_ );
            currentCommandYawScale_ = 
                hydroutil::randNumInclusive( 1.0-noiseLevelRotational_, 1.0+noiseLevelRotational_ );
        }
        else
        {
            assert( false && "not implemented." );
        }
        lastScaleFactorChangeTime_ = orcaice::getNow();

        cout<<"TRACE(platform2dI.cpp): Scale factors (lin,rot): " << currentCommandVelScale_ << ", " << currentCommandYawScale_ << endl;
    }
}

}
