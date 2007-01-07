/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCAMISC_RECURSIVE_GAUSSIAN_H
#define ORCAMISC_RECURSIVE_GAUSSIAN_H

namespace orcamisc
{

/*
 *  @brief 1-dimensional Gaussian model.
 *
 *  Recursively estimates 1D Guassian statistics by adding one sample at a time.
 *
 *  Setting @p maxCount to a finite positive value will limit the weigting
 *  assigned to the current estimate of the mean and covariance. This is convinient
 *  for modelling non-stationary processes. No maximum count is set when @p maxCount
 *  is negative. Integer wrap-around is not handled, so the maximum count is limited
 *  by the maximum integer.
 *  
 */
class RecursiveGaussian
{
public:

    //! Sets initial estimate.
    RecursiveGaussian( double mean=0.0, double cov=1.0, int maxCount=100 );

    //! Equivalent to calling the constructor. Resets sample count to zero.
    void reset( double mean=0.0, double cov=1.0, int maxCount=100 );
    
    //! Adds a single sample.
    void addSample( double sample );

    //! Returns current estimate of the mean of the distribution.
    double mean() const { return mean_; };
    
    //! Returns current estimate of the covariance of the distribution.
    double cov() const { return cov_; };

    //! Returns current estimate of the standard deviation of the distribution.
    double std() const;
    
    //! Returns current sample count.
    int sampleCount() const { return sampleCount_; };

private:
    double mean_;
    double cov_;
    
    int sampleCount_;
    int maxSampleCount_;

};

} // end namespace

#endif
