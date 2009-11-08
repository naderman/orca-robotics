#ifndef ORCAPUBLISH_OGMAPPUBLISHER_H
#define ORCAPUBLISH_OGMAPPUBLISHER_H

#include <hydropublish/ogmappublisher.h>
#include <orcaifaceimpl/ogmap.h>
#include <orcaogmap/orcaogmap.h>
#include <orcaice/timeutils.h>

namespace orcapublish {

//
// @author Alex Brooks
//
class OgMapPublisher : public hydropublish::OgMapPublisher
{
public: 

    OgMapPublisher( const orcaice::Context &context, const std::string &name )
        : impl_(new orcaifaceimpl::OgMapImpl(context,name))
        {
        }

    inline void init() { impl_->initInterface(); }
    inline void localSetAndSend( const hydroogmap::OgMap &ogMap )
        { 
            orcaogmap::convert( ogMap, orcaOgMapData_ );
            orcaOgMapData_.timeStamp = orcaice::getNow();
            impl_->localSetAndSend( orcaOgMapData_ );
        }

private: 

    orcaifaceimpl::OgMapImplPtr impl_;
    orca::OgMapData orcaOgMapData_;
};

class OgMapPublisherFactory : public hydropublish::OgMapPublisherFactory
{
public:
    OgMapPublisherFactory( const orcaice::Context &context )
        : context_(context)
        {}

    hydropublish::OgMapPublisherPtr createPublisher( const std::string &name )
        { return hydropublish::OgMapPublisherPtr(new OgMapPublisher( context_, name )); }

private:

    orcaice::Context context_;
};

}

#endif
