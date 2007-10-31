/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>

#include "component.h"
#include "platform2dI.h"
#include "position2dconsumerI.h"
#include "multiplicativenoisemodel.h"

using namespace orca;
using namespace std;

namespace noisyrobot2d {

Component::Component() :
    orcaice::Component( "NoisyRobot2d" )
{
}

Component::~Component()
{
    delete noiseModel_;
}

// warning: this function returns after it's done, all variable that need to be permanet must
//          be declared as member variables.
void
Component::start()
{
    std::string prefix = tag();
    prefix += ".Config.";

    double velNoiseLevel = orcaice::getPropertyAsDoubleWithDefault( properties(), prefix+"VelNoiseLevel", 0.25 );
    double yawNoiseLevel = orcaice::getPropertyAsDoubleWithDefault( properties(), prefix+"YawNoiseLevel", 0.35 );
    std::string noiseLevelDistributionType = orcaice::getPropertyWithDefault( properties(), prefix+"NoiseLevelDistributionType", "uniform" );
    double constantNoiseLevelTime = orcaice::getPropertyAsDoubleWithDefault( properties(), prefix+"ConstantNoiseLevelTime", 10.0 );

    MultiplicativeNoiseModel::NoiseLevelDistributionType noiseType;
    if ( noiseLevelDistributionType == "uniform" )
        noiseType = MultiplicativeNoiseModel::DISTRIBUTION_UNIFORM;
    else if ( noiseLevelDistributionType == "gaussian" )
        noiseType = MultiplicativeNoiseModel::DISTRIBUTION_GAUSSIAN;
    else
    {
        cout << "ERROR(component.cpp): Unknown NoiseLevelDistributionType: " << noiseLevelDistributionType << endl;
        exit(1);
    }

    noiseModel_ = new MultiplicativeNoiseModel( noiseType,
                                                velNoiseLevel,
                                                yawNoiseLevel,
                                                constantNoiseLevelTime );

    // provided interface
    Platform2dI *platformInterface = new Platform2dI( "MyPlatform2d", "RealPlatform2d", *noiseModel_, context() );
    Ice::ObjectPtr obj = platformInterface;
    orcaice::createInterfaceWithTag( context(), obj, "MyPlatform2d" );

    // required interface
    Ice::ObjectPtr consumer = new Position2dConsumerI( *platformInterface );
    orca::Position2dConsumerPrx callbackPrx =
        orcaice::createConsumerInterface<orca::Position2dConsumerPrx>( context(), consumer );

    platformInterface->realRobotPrx()->subscribe( callbackPrx );
    cout<<"TRACE(component.cpp): activating." << endl;
    activate();
}

void
Component::stop()
{
}

}
