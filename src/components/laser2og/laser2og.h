/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef _ORCA2_LASER2OG_H_
#define _ORCA2_LASER2OG_H_

// orca objects
#include <orca/laserscanner2d.h>
#include <orca/ogfusion.h>
#include <vector>
#include <hydronavutil/pose.h>
#include "ogsensormodel.h"
#include <orcaogfusion/orcaogfusion.h>

namespace laser2og
{

class Laser2Og
{
public:
    Laser2Og( ogfusion::MapConfig &mapConfig,
              OgLaserModelConfig  &sensorConfig );
    ~Laser2Og();

    orca::OgObservation process( const hydronavutil::Pose &pose, const orca::RangeScanner2dData & scan );

    // If the vehicle pose uncertainty is greater than this, we shouldn't process sensor data
    double positionStdDevMax() const { return sensorModel_->posStDevMax(); }
    double headingStdDevMax() const { return sensorModel_->hedStDevMax(); }

private:
    
    orca::OgObservation getObs( const ogfusion::OgBuffer &buffer );

    // sensor config
    double maxLaserRange_;
        
    // sensor model
    OgSensorModel* sensorModel_;

    // map config
    double mapResX_;
    double mapResY_;
    int mapSizeX_;
    int mapSizeY_;
    double mapOriginX_;
    double mapOriginY_;
};

} // namespace

#endif
