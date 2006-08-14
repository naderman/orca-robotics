/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef _ORCA2_LASER2OG_H_
#define _ORCA2_LASER2OG_H_

// orca objects
#include <orca/laser.h>
#include <orca/localise2d.h>
#include <orca/ogfusion.h>

#include <vector>
#include <orcaogfusion/orcaogfusion.h>

#include "ogsensormodel.h"

namespace laser2og
{

// forward declaration
class OgSensorModel;

class Laser2Og
{
public:
    Laser2Og(ogfusion::MapConfig& mapConfig, OgLaserModelConfig& sensorConfig);
    ~Laser2Og();

    int process( const orca::Localise2dData &pose, const orca::RangeScannerData & scan );
    int getObs( orca::OgObservation &obs );

private:
    
    // sensor config
    double laserRange_;
        
    // sensor model
    OgSensorModel* sensorModel_;

    // map config
    double mapResX_;
    double mapResY_;
    int mapSizeX_;
    int mapSizeY_;
    double mapOriginX_;
    double mapOriginY_;

    ogfusion::OgBuffer buffer_;
   
};

} // namespace

#endif
