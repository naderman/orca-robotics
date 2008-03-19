/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#include <orca/ogfusion.h>
#include <gbxsickacfr/gbxutilacfr/mathdefs.h>

#include "ogcellops.h"

using namespace std; // for math functions in QNX

orca::OgCellLikelihood 
ogfusion::add (const orca::OgCellLikelihood& f, const orca::OgCellLikelihood& g )
{

    //occ_ = occ_*feature->occ_  / ( occ_*feature->occ_ + (1.0-occ_)*(1.0-feature->occ_) );
    // same but slightly optimized:

    double d = f.likelihood * g.likelihood;
    orca::OgCellLikelihood out;
    out.x=f.x;
    out.y=f.y;
    double resultLikelihood = d / (2.0*d + 1.0 - f.likelihood - g.likelihood );

    CLIP_TO_LIMITS(ogLimitLowD, resultLikelihood, ogLimitHighD);
    out.likelihood = resultLikelihood;
    return out;
}
