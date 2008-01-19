/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef _OGCELLOPS_H_
#define _OGCELLOPS_H_

#include <orca/ogfusion.h>
#include <orcaogmap/orcaogmap.h>

namespace ogfusion{

    const double ogLimitHighD = 1.0 - 1.0E-2;
    const double ogLimitLowD = 1.0E-2;
    const unsigned char ogLimitHighUC = 253;
    const unsigned char ogLimitLowUC = 1;
    
    //! Fuses two probability distributions and performs normalization.
    orca::OgCellLikelihood add(const orca::OgCellLikelihood& f, const orca::OgCellLikelihood& g );

}
 
#endif
