/*
 *  
 *  Australian Centre for Field Robotics
 *  Sydney, Australia
 *  www.acfr.usyd.edu.au
 *  
 */

#ifndef _BRUCE2_LASER2OG_H_
#define _BRUCE2_LASER2OG_H_

// orca objects
#include <orca/bros1.h>
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
        
    //! Sensor model.
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
