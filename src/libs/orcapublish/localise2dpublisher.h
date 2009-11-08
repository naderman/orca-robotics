#ifndef ORCAPUBLISH_LOCALISE2DPUBLISHER_H
#define ORCAPUBLISH_LOCALISE2DPUBLISHER_H

#include <orcaifaceimpl/localise2d.h>
#include <hydropublish/localise2dpublisher.h>

namespace orcapublish {

//
// @author Alex Brooks
//
class Localise2dPublisher : public hydropublish::Localise2dPublisher
{
public: 

    Localise2dPublisher( const orca::VehicleGeometryDescriptionPtr &geom,
                         const orcaice::Context                    &context, 
                         const std::string                         &name );

    Localise2dPublisher( const orcaice::Context &context, 
                         const std::string      &name );

    void init() { impl_->initInterface(); }
    void localSetAndSend( const hydronavutil::Gmm &gmm,
                          const hydrotime::Time   &timeStamp );
    void localSetAndSend( const hydronavutil::Pose &pose,
                          const hydrotime::Time    &time = hydrotime::Time(0,0) );
                          

private: 

    orcaifaceimpl::Localise2dImplPtr impl_;
    orca::Localise2dData             orcaLocalise2d_;
};

class Localise2dPublisherFactory : public hydropublish::Localise2dPublisherFactory
{
public:
    Localise2dPublisherFactory( const orcaice::Context &context )
        : context_(context)
        {}

    hydropublish::Localise2dPublisherPtr createPublisher( 
        const hydronavutil::CylindricalGeometryDescription &cylindricalDescription,
        const std::string                                  &name );

    hydropublish::Localise2dPublisherPtr createPublisher( const std::string &name );

private:

    orcaice::Context context_;
};

}

#endif
