/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#include "gmm.h"
#include <cstddef>
#include <iostream>
#include <sstream>


using namespace std;

namespace hydronavutil {

Gmm::Gmm( const std::vector<Gaussian> &components,
          const std::vector<double>   &weights )
    : components_(components),
      weights_(weights)
{
    checkIsSane();
}

Gmm::Gmm( const Pose &mean, const Cov3d &cov )
{
    components_.push_back( Gaussian( mean, cov ) );
    weights_.push_back( 1.0 );
    checkIsSane();
}

void
Gmm::checkIsSane()
{
    if ( weights_.size() != components_.size() )
    {
        stringstream ss;
        ss << "Gmm: weights and components are not of same size: weights.size()="<<weights_.size()<<", components.size()="<<components_.size();
        throw hydronavutil::Exception( ERROR_INFO, ss.str() );
    }

    if ( size() == 0 )
    {
        stringstream ss;
        ss << "Gmm: has zero components!";
        throw hydronavutil::Exception( ERROR_INFO, ss.str() );        
    }

    double weightSum = 0.0;
    for ( size_t i=0; i < size(); i++ )
    {
        weightSum += weights_[i];

        if ( weights_[i] < 0.0 )
        {
            stringstream ss;
            ss << "Gmm: weight["<<i<<"] < 0.0: " << weights_[i];
            throw hydronavutil::Exception( ERROR_INFO, ss.str() );
        }
    }
    const double EPS = 1e-9;
    if ( fabs( weightSum-1.0 ) > EPS )
    {
        stringstream ss;
        ss << "Gmm: weights do not sum to 1.0.  weightSum = " << weightSum << ", 1.0-weightSum = " << 1.0-weightSum;
        throw hydronavutil::Exception( ERROR_INFO, ss.str() );
    }

    for ( size_t i=0; i < size(); i++ )
    {
        if ( !components_[i].cov().isSPD() )
        {
            stringstream ss;
            ss << "Gmm: component " << i << " is not SPD: " << components_[i].cov();
            throw hydronavutil::Exception( ERROR_INFO, ss.str() );
        }
    }
}

void
Gmm::addComponent( const Gaussian &comp, double weight )
{
    components_.push_back( comp );
    weights_.push_back( weight );
    normaliseWeights();
}

void
Gmm::normaliseWeights()
{
    // Compute the weight sum
    double weightSum = 0.0;
    for ( size_t i=0; i < weights_.size(); i++ )
    {
        weightSum += weights_[i];
    }
    assert( weightSum > 0.0 );

    // Normalise
    for ( size_t i=0; i < weights_.size(); i++ )
    {
        weights_[i] /= weightSum;
    }
}

int
Gmm::mlComponentI() const
{
    int    mlI  = -1;
    double wMax = -1;

    for ( size_t i=0; i < size(); i++ )
    {
        if ( weights_[i] > wMax )
        {
            wMax = weights_[i];
            mlI  = i;
        }
    }

    if ( mlI == -1 )
    {
        assert( false && "Failed to find mlComponent" );
    }
    return mlI;
}

const Gaussian&
Gmm::mlComponent() const
{
    return components_[mlComponentI()];
}

////////////////////////////////////////////////////////////////////////////////
//    Non-member functions
////////////////////////////////////////////////////////////////////////////////

bool isUncertain( const Gmm &gmm,
                  double maxLinearVar,
                  double maxRotationalVar,
                  int    maxComponents )
{
    if ( (int)(gmm.size()) > maxComponents ) return true;

    const Gaussian &g = gmm.mlComponent();

    if ( g.cov().xx() > maxLinearVar ||
         g.cov().yy() > maxLinearVar ||
         g.cov().tt() > maxRotationalVar )
    {
        return true;
    }
    else
    {
        return false;
    }
}
std::ostream &operator<<( std::ostream &s, const Gaussian &g )
{
    return s <<"{"<<g.mean()<<", "<<g.cov()<<"}";
}

std::ostream &operator<<( std::ostream &s, const Gmm &g )
{
    s << g.size() << " components:" << endl;
    for ( size_t i=0; i < g.size(); i++ )
    {
        s << "  "<<i<<": w="<<g.weights(i)<<": " << g.components(i)<<endl;
    }
    return s;
}

}

