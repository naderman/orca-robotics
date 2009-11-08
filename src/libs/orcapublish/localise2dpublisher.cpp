#include "localise2dpublisher.h"
#include <iostream>
#include <orcaice/orcaice.h>
#include <orcanavutil/orcanavutil.h>
#include <orcaobj/bros1.h>
#include <orcaice/timeutils.h>

using namespace std;

namespace orcapublish {

//////////////////////////////////////////////////////////////////////

Localise2dPublisher::Localise2dPublisher( const orca::VehicleGeometryDescriptionPtr &geom,
                                          const orcaice::Context                    &context, 
                                          const std::string                         &name )
{
    impl_ = new orcaifaceimpl::Localise2dImpl( geom,context,name );
}

Localise2dPublisher::Localise2dPublisher( const orcaice::Context                    &context, 
                                          const std::string                         &name )
    
{
    // Create a bogus vehicle geom description

    orca::VehicleGeometryCuboidDescriptionPtr descr = new orca::VehicleGeometryCuboidDescription;
    descr->type = orca::VehicleGeometryCuboid;
    descr->size.l = 0.1;
    descr->size.w = 0.1;
    descr->size.h = 0.1;
    descr->platformToGeometryTransform = orcaobj::zeroFrame3d();
    
    impl_ = new orcaifaceimpl::Localise2dImpl(descr,context,name);
}

void
Localise2dPublisher::localSetAndSend( const hydronavutil::Gmm &gmm,
                                      const hydrotime::Time   &timeStamp )
{
    impl_->localSetAndSend( orcanavutil::convert( gmm, timeStamp.seconds(), timeStamp.useconds() ) );
}
void
Localise2dPublisher::localSetAndSend( const hydronavutil::Pose &pose,
                                      const hydrotime::Time    &time )
{
    orca::Localise2dData localiseData;
    localiseData.timeStamp.seconds = time.seconds();
    localiseData.timeStamp.useconds = time.useconds();

    orca::Pose2dHypothesis hyp;
    hyp.mean = orcanavutil::convert( pose );
    hyp.weight = 1.0;
    hyp.cov = orcanavutil::diagonalCovariance( 0.01, 0.01 );
    localiseData.hypotheses.push_back( hyp );

    impl_->localSetAndSend( localiseData );
}

hydropublish::Localise2dPublisherPtr
Localise2dPublisherFactory::createPublisher( 
        const hydronavutil::CylindricalGeometryDescription &cylindricalDescription,
        const std::string                                  &name )
{
    return hydropublish::Localise2dPublisherPtr(
        new Localise2dPublisher( orcanavutil::convert(cylindricalDescription),
                                 context_,
                                 name ) );
}

hydropublish::Localise2dPublisherPtr 
Localise2dPublisherFactory::createPublisher( const std::string &name )
{
    return hydropublish::Localise2dPublisherPtr(new Localise2dPublisher( context_, name ));
}


}

