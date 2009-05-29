/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <gbxutilacfr/exceptions.h>
#include "localise2d.h"
#include "datetime.h"
#include "stochastic.h"

using namespace std;

namespace orcaobj
{

void normalise( orca::Localise2dData& obj )
{
    float weightSum = 0.0;
    for ( unsigned int i=0; i < obj.hypotheses.size(); i++ )
    {
        weightSum += obj.hypotheses[i].weight;
    }
    assert( weightSum > 0.0 );

    for ( unsigned int i=0; i < obj.hypotheses.size(); i++ )
    {
        obj.hypotheses[i].weight /= weightSum;
    }
}

const orca::Pose2dHypothesis& mlHypothesis( const orca::Localise2dData& obj )
{
    float maxWeight = -1;
    int   mlI       = -1;
    for ( unsigned int i=0; i < obj.hypotheses.size(); i++ )
    {
        if ( obj.hypotheses[i].weight > maxWeight )
        {
            maxWeight = obj.hypotheses[i].weight;
            mlI = i;
        }
    }
#ifndef NDEBUG
    if ( mlI < 0 )
    {
        std::stringstream ss;
        ss << "Dodgy Localise2dDataPtr: " << orcaobj::toString(obj);
        throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
    }
#endif
    return obj.hypotheses[mlI];
}

bool localisationIsUncertain( const orca::Localise2dData &localiseData,
                              double linearThreshold )
{
    // Some dodgy heuristics
    if ( localiseData.hypotheses.size() > 2 )
        return true;

    const orca::Pose2dHypothesis h = orcaobj::mlHypothesis( localiseData );
    if ( h.cov.xx > linearThreshold ||
         h.cov.yy > linearThreshold )
        return true;

    return false;
}

std::string toString( const orca::Localise2dData& obj )
{
    std::ostringstream s;
    s << toString(obj.timeStamp)
        << " Localise2d ["<<obj.hypotheses.size()<<" elements] [weight] (x,y,a(deg) (pxx,pxy,pyy,pxt,pyt,pyy(deg)) :"<<endl;
    for ( unsigned int i=0; i < obj.hypotheses.size(); i++ )
    {
        const orca::Pose2dHypothesis &h = obj.hypotheses[i];
        s << "  " << i << ": [" 
          << h.weight << "] (" 
          << h.mean.p.x << ","
          << h.mean.p.y << ","
          << h.mean.o*180.0/M_PI << "deg) "
          << toString(h.cov) << endl;
    }
    return s.str();
}

} // namespace
