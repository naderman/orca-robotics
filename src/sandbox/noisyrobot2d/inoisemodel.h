#ifndef INOISEMODEL_H
#define INOISEMODEL_H

#include <orca/platform2d.h>

//
// @author Alex Brooks
//
class INoiseModel
{

public: 

    virtual ~INoiseModel() {};

    virtual void perturbCommand( orca::Velocity2dCommand& command )=0;
    virtual void perturbOdom( orca::Position2dData& odom )=0;

private: 


};

#endif
