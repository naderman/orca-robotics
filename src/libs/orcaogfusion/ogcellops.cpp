/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#include <orca/ogfusion.h>
#include <orcaobj/mathdefs.h>

#include "ogcellops.h"

//! Fuses two probability distributions and performs normalization.
orca::OgCellLikelihood 
ogfusion::add (const orca::OgCellLikelihood& f, const orca::OgCellLikelihood& g )
{

    //occ_ = occ_*feature->occ_  / ( occ_*feature->occ_ + (1.0-occ_)*(1.0-feature->occ_) );
    // same but slightly optimized:

    double d = f.likelihood * g.likelihood;
    orca::OgCellLikelihood out;
    out.x=f.x;
    out.y=f.y;
    out.likelihood = d / (2.0*d + 1.0 - f.likelihood - g.likelihood );

    out.likelihood = CHECK_LIMITS(ogLimitHighD, out.likelihood, ogLimitLowD);
    return out;
}
 
//! Fuses two a cell from a map with likelihood performing normalization.
int 
ogfusion::add (orcaogmap::OgMap& m, const orca::OgCellLikelihood& f )
{
    unsigned char cell;
    if(!m.tryGridCell( f.x, f.y, cell ))
        return -1;

    double g = (double)cell/254.0;
    double d = f.likelihood * g;
    double out = d / (2.0*d + 1.0 - f.likelihood - g );

    unsigned char out_uc = (unsigned char)floor(out*254.0+0.5);
    // do not allow to reach "0" or "1"
    m.gridCell( f.x, f.y ) = CHECK_LIMITS(ogLimitHighUC, out_uc, ogLimitLowUC);

    return 0;
}
