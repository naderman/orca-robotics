#ifndef LOCALNAV_ALGORITHMEVALUATOR_H
#define LOCALNAV_ALGORITHMEVALUATOR_H

#include "testsim/simulator.h"
#include <list>

namespace localnav {

//
// @brief For evaluating algorithms in simulation
//
// @author Alex Brooks
//
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

    std::list<hydronavutil::Pose> poseHistory_;
};

}

#endif
