#ifndef LOCALNAV_ALGORITHMEVALUATOR_H
#define LOCALNAV_ALGORITHMEVALUATOR_H

#include "testsim/simulator.h"

namespace localnav {

//!
//! @author Alex Brooks
//!
class AlgorithmEvaluator
{

public: 

    AlgorithmEvaluator( const std::string &saveFile )
        : numIterations_(0),
          cumDecisionTime_(0),
          cumObstacleCost_(0),
          saveFile_(saveFile)
        {}
    ~AlgorithmEvaluator();

    void evaluate( double timeToMakeDecision,
                   const Simulator &simulator );

private: 

    int    numIterations_;
    double cumDecisionTime_;
    double cumObstacleCost_;
    const std::string saveFile_;
};

}

#endif
