/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp,
 *               George Mathews, Stef Williams.
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef DOOREXTRACTOR__H
#define DOOREXTRACTOR__H

#include <orca/laserscanner2d.h>
#include <orca/polarfeature2d.h>
#include "iextractor.h"
#include <orcaice/context.h>

namespace laserfeatures {

class DoorExtractor : public IExtractor
{

public: 

    DoorExtractor( orcaice::Context context ) {}

//     Adds laser features to the 'features' data structure
      void addFeatures( const orca::LaserScanner2dDataPtr &laserData,
                        orca::PolarFeature2dDataPtr &features );   

private: 
        
};

}

#endif
