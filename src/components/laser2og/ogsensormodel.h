/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef OG_SENSOR_MODEL_H
#define OG_SENSOR_MODEL_H

#include <vector>

namespace laser2og
{

//! A structure to describe the likelihood function of Laser-to-OG observation model.
typedef struct
{
    int size;
    double occupMax;
    double emptyMax;
    double rangeMax;
    double rangeStDev;
    double rangeStDevMax;
    double posStDevMax;
    double hedStDevMax;
    double angleIncrement;
} OgLaserModelConfig; //og_laser_model_config_t;

/*!
    Likelihood function for certainty grids (CG).

  @author Alex Makarenko
*/
class OgSensorModel
{
public: 
    OgSensorModel( const OgLaserModelConfig & config );
    ~OgSensorModel();
    
    double likelihood( const double rangeToCell, const double rangeToReturn, const double posSD, const double headSD);

    // debug
    void print();

    double hedStDevMax() {return config_.hedStDevMax;};
    double posStDevMax() {return config_.posStDevMax;};

private:
    // range to return
    std::vector<double> r_;
    // likelihood when return is within max range
    std::vector<double> pIn_;
    // likelihood when return is outside of max range
    std::vector<double> pOut_;   
    
    OgLaserModelConfig config_;
    
    void init();

};

} // namespace

#endif
