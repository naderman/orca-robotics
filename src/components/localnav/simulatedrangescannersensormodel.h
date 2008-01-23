/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef SIMULATEDRANGESCANNERSENSORMODEL_H
#define SIMULATEDRANGESCANNERSENSORMODEL_H

namespace localnav {

#include "isensormodel.h"
#include "isensordata.h"

//
// @author Alex Brooks
//
class SimulatedRangeScannerSensorModel : public ISensorModel
{

public: 

    SimulatedRangeScannerSensorModel();
    ~SimulatedRangeScannerSensorModel();

private: 


};

}

#endif
