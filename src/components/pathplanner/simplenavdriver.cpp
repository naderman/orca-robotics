/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>

#include <orcapathplan/orcapathplan.h>
#include <orcamisc/orcamisc.h>

#include "simplenavdriver.h"

using namespace std;

namespace pathplanner {
    
SimpleNavDriver::SimpleNavDriver( const orcaogmap::OgMap &ogMap,
                                  double robotDiameterMetres,
                                  double traversabilityThreshhold,
                                  bool doPathOptimization )
    : pathPlanner_(NULL),
      genericDriver_(NULL)
{
    cout<<"TRACE(simplenavdriver.cpp): SimpleNavDriver()" << endl;
    pathPlanner_ = new orcapathplan::SimpleNavPathPlanner( ogMap,
                                                           robotDiameterMetres,
                                                           traversabilityThreshhold,
                                                           doPathOptimization );
    genericDriver_ = new GenericDriver( pathPlanner_,
                                        ogMap,
                                        robotDiameterMetres,
                                        traversabilityThreshhold,
                                        doPathOptimization );
    
}
    
SimpleNavDriver::~SimpleNavDriver()
{
    if (pathPlanner_!=NULL) delete pathPlanner_;
    if (genericDriver_!=NULL) delete genericDriver_;
}
    

void SimpleNavDriver::computePath( const orca::PathPlanner2dTask& task,
                                   orca::PathPlanner2dData& pathData )
{
    genericDriver_->computePath( task, pathData );
}

}


