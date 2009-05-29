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
#include "localise3d.h"
#include "datetime.h"

using namespace std;

namespace orcaobj
{

const orca::Pose3dHypothesis & mlHypothesis( const orca::Localise3dData& obj )
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
        ss << "Dodgy Localise3dDataPtr: " << orcaobj::toString(obj);
        throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
    }
#endif
    return obj.hypotheses[mlI];
}

std::string toString( const orca::Localise3dData& obj )
{
    std::ostringstream s;
    s << toString(obj.timeStamp)
        << " Localise3d ["<<obj.hypotheses.size()<<" elements] [weight] (x,y,z,r(deg),p(deg),a(deg) (pxx,pxy,pxz,pyy,pyz,pzz,pxr,pxp,pxa,pyr,pyp,pya,pzr,pzp,pza,prr,prp,pra,ppp,ppa,paa(deg)) :"<<endl;
    for ( unsigned int i=0; i < obj.hypotheses.size(); i++ )
    {
        const orca::Pose3dHypothesis &h = obj.hypotheses[i];
        s << "  " << i << ": [" 
          << h.weight << "] (" 
          << setprecision(15)
          << h.mean.p.x << ","
          << h.mean.p.y << ","
          << h.mean.p.z << ","
          << h.mean.o.r*180.0/M_PI << "deg, "
          << h.mean.o.p*180.0/M_PI << "deg, "
          << h.mean.o.y*180.0/M_PI << "deg) ("
          << h.cov.xx << ","
          << h.cov.xy << ","
          << h.cov.xz << ","
          << h.cov.yy << ","
          << h.cov.yz << ","
          << h.cov.zz << ","
          << h.cov.xr << ","
          << h.cov.xp << ","
          << h.cov.xa << ","
          << h.cov.yr << ","
          << h.cov.yp << ","
          << h.cov.ya << ","
          << h.cov.zr << ","
          << h.cov.zp << ","
          << h.cov.za << ","
          << h.cov.rr*180.0/M_PI << "deg, "
          << h.cov.rp*180.0/M_PI << "deg, "
          << h.cov.ra*180.0/M_PI << "deg, "
          << h.cov.pp*180.0/M_PI << "deg, "
          << h.cov.pa*180.0/M_PI << "deg, "
          << h.cov.aa*180.0/M_PI << "deg)" << endl;
    }
    return s.str();
}

} // namespace
