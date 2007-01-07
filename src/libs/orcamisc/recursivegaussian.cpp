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
#include <cmath>

#include "recursivegaussian.h"

using namespace std;

namespace orcamisc
{

RecursiveGaussian::RecursiveGaussian( double mean, double cov, int maxCount )
    : mean_(mean),
      cov_(cov),
      sampleCount_(0),
      maxSampleCount_(maxCount)
{
}

void RecursiveGaussian::reset( double mean, double cov, int maxCount )
{
    mean_ = mean;
    cov_ = cov;
    sampleCount_ = 0;
    maxSampleCount_ = maxCount;
}

void RecursiveGaussian::addSample( double sample )
{
    if ( sampleCount_<maxSampleCount_ || maxSampleCount_<0 ) {
        ++sampleCount_;
    }
    
    if ( sampleCount_ == 1 )
    {
        // For a uniform distribution, we've probably only seen half the time interval
        mean_ = sample;
    }
    else
    {
        mean_ = ( (sampleCount_-1)*mean_ + sample ) / (sampleCount_);
        if ( sampleCount_ == 2 )
        {
            // Normalise by N-1, not N.
            cov_ = ( (sample-mean_)*(sample-mean_) ) / (sampleCount_-1);
        }
        else
        {
            cov_ = ( (sampleCount_-1)*cov_ + (sample-mean_)*(sample-mean_) ) / (sampleCount_);
        }
    }

    // debug
    cout<<"mean, std       : "<<mean_<<", "<<std()<<endl;
    cout<<"count           : "<<sampleCount_<<endl<<endl;
}

double RecursiveGaussian::std() const
{
    return sqrt(cov_);
}

} // namespace
