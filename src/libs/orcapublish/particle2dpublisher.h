#ifndef ORCAPUBLISH_PARTICLE2DPUBLISHER_H
#define ORCAPUBLISH_PARTICLE2DPUBLISHER_H

#include <hydropublish/particle2dpublisher.h>
#include <orcaifaceimpl/particle2d.h>

namespace orcapublish {

//!
//! @author Alex Brooks
//!
class Particle2dPublisher : public hydropublish::Particle2dPublisher
{
public: 

    Particle2dPublisher( const orcaice::Context &context, const std::string &name );

    void init() { impl_->initInterface(); }
    void localSetAndSend( const std::vector<hydropublish::Particle2d> &particles );

private: 

    orcaifaceimpl::Particle2dImplPtr impl_;
    orca::Particle2dData orcaData_;
};

class Particle2dPublisherFactory : public hydropublish::Particle2dPublisherFactory
{
public:
    Particle2dPublisherFactory( const orcaice::Context &context )
        : context_(context)
        {}

    hydropublish::Particle2dPublisherPtr createPublisher( const std::string &name )
        { return hydropublish::Particle2dPublisherPtr(new Particle2dPublisher( context_, name )); }

private:

    orcaice::Context context_;
};

}

#endif
