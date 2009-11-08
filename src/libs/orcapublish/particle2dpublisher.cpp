#include "particle2dpublisher.h"
#include <iostream>

using namespace std;

namespace orcapublish {

Particle2dPublisher::Particle2dPublisher( const orcaice::Context &context, const std::string &name )
    : impl_(new orcaifaceimpl::Particle2dImpl(context,name))
{
}

void
Particle2dPublisher::localSetAndSend( const std::vector<hydropublish::Particle2d> &particles )
{
    orcaData_.particles.resize( particles.size() );

    for ( unsigned int i=0; i < particles.size(); i++ )
    {
        orcaData_.particles[i].pose.p.x = particles[i].x;
        orcaData_.particles[i].pose.p.y = particles[i].y;
        orcaData_.particles[i].pose.o   = particles[i].theta;
        orcaData_.particles[i].weight   = particles[i].weight;
    }

    impl_->localSetAndSend( orcaData_ );
}

}

