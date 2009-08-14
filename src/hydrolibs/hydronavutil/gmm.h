/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef HYDRONAVUTIL_GMM_H
#define HYDRONAVUTIL_GMM_H

#include <vector>
#include <hydronavutil/cov3d.h>
#include <hydronavutil/pose.h>
#include <assert.h>

namespace hydronavutil {

//!
//! @brief A single component of a Gmm
//!
//! @author Alex Brooks
//!
class Gaussian
{

public:

    Gaussian() {}
    Gaussian( const Pose &mean, const Cov3d &cov )
        : mean_(mean), cov_(cov)
        {}

    // non-const versions
    Pose   &mean() { return mean_; }
    Cov3d  &cov()  { return cov_; }

    // const versions
    const Pose &mean() const { return mean_; }
    const Cov3d &cov() const { return cov_; }

private:

    // The mean pose
    Pose  mean_;

    // The covariance of the gaussian (x,y,theta)
    Cov3d cov_;

    // The weight of this component in the mixture
    double weight_;
};
std::ostream &operator<<( std::ostream &s, const Gaussian &g );

//!
//! @brief A Gaussian Mixture Model (GMM) representing a distribution over 2D robot poses.
//!
//! @author Alex Brooks
//!
class Gmm
{

public: 

    Gmm() {}

    //! Convenience function for constructing a Gmm of one component
    Gmm( const Pose &mean, const Cov3d &cov );

    //! Construct arbitrary Gmm's
    Gmm( const std::vector<Gaussian> &components,
         const std::vector<double>   &weights );

    unsigned int size() const { return components_.size(); }

    //! const access
    const Gaussian &components(int i) const
        {
            assert( i < (int)(components_.size()) );
            return components_[i];
        }
    //! const access
    double weights(int i) const
        {
            assert( i < (int)(weights_.size()) );
            return weights_[i];
        }

    //! non-const access
    Gaussian &components(int i)
        {
            assert( i < (int)(components_.size()) );
            return components_[i];
        }
    //! non-const access
    double &weights(int i)
        {
            assert( i < (int)(weights_.size()) );
            return weights_[i];
        }

    //! Returns the index of the most likely component
    int mlComponentI() const;

    //! Returns a const ref to the most likely component.
    const Gaussian &mlComponent() const;

    //! Renormalises to ensure the weights sum to one.
    void addComponent( const Gaussian &comp, double weight );

    // throws exceptions if not sane
    void checkIsSane();

private: 

    // Makes the weights sum to 1.0
    void normaliseWeights();

    std::vector<Gaussian> components_;
    std::vector<double>   weights_;
};
std::ostream &operator<<( std::ostream &s, const Gmm &g );

//! Checks whether the Gmm is uncertain, based on dodgy heuristics.
//! The linear and rotational variances refer to the diagonal elements of the most likely component.
bool isUncertain( const Gmm &gmm,
                  double maxLinearVar     = 5.0,
                  double maxRotationalVar = (M_PI/2.0),
                  int    maxComponents    = 2 );

}

#endif
