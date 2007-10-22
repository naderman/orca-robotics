#ifndef MULTIPLICATIVENOISEMODEL_H
#define MULTIPLICATIVENOISEMODEL_H

#include "inoisemodel.h"
#include <hydronavutil/odometrydifferentiator.h>

namespace noisyrobot2d {

//!
//! Holds the multipliers constant for constantMultiplierTime seconds, then
//! samples new ones.
//!
//! Setting constantMultiplierTime to zero gets new multipliers every time
//! a command is sent.
//!
//! Noise types are:
//!  - DISTRIBUTION_UNIFORM:  Selects multipliers from a uniform distribution
//!                           in the range [ 1-noiseLevel, 1+noiseLevel ]
//!  - DISTRIBUTION_GAUSSIAN: Selects multipliers from a normal distribution
//!                           with mean 1, standard deviation noiseLevel
//!
//! @author Alex Brooks
//!
class MultiplicativeNoiseModel : public INoiseModel
{

public: 

    enum NoiseLevelDistributionType {
        DISTRIBUTION_UNIFORM,
        DISTRIBUTION_GAUSSIAN
    };

    MultiplicativeNoiseModel( NoiseLevelDistributionType   noiseLevelType, 
                              double                       noiseLevelLinear,
                              double                       noiseLevelRotational,
                              double                       constantMultiplierTime );

    void perturbCommand( orca::Velocity2dCommand& command );
    void perturbOdom( ::orca::Position2dData& odom );

private: 

    void getNewScaleFactors();

    double                         currentCommandVelScale_;
    double                         currentCommandYawScale_;
    orca::Time                     lastScaleFactorChangeTime_;
    double                         constantMultiplierTime_;

    hydronavutil::OdometryDifferentiator odomDifferentiator_;
    hydronavutil::Offset                 apparentOdom_;

    NoiseLevelDistributionType          noiseLevelType_;
    double                              noiseLevelLinear_;
    double                              noiseLevelRotational_;
};

}

#endif
