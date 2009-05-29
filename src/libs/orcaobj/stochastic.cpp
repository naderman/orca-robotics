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
#include <gbxutilacfr/mathdefs.h>
#include "stochastic.h"

using namespace std;

namespace orcaobj
{

void 
setInit( orca::Covariance2d& obj, double pxx, double pxy, double pyy, double pxt, double pyt, double ptt )
{
    obj.xx = pxx;
    obj.xy = pxy;
    obj.yy = pyy;
    obj.xt = pxt;
    obj.yt = pyt;
    obj.tt = ptt;
}

std::string
toString( const orca::Covariance2d &cov )
{
    stringstream ss;
    ss << "("
       << cov.xx << ","
       << cov.xy << ","
       << cov.yy << ","
       << cov.xt << ","
       << cov.yt << ","
       << cov.tt*180.0/M_PI << "deg)" << endl;
    return ss.str();
}

} // namespace
