#ifndef ORCAPUBLISH_QGRAPHICS2DPUBLISHER_H
#define ORCAPUBLISH_QGRAPHICS2DPUBLISHER_H

#include <hydropublish/qgraphics2dpublisher.h>
#include <orcaifaceimpl/qgraphics2d.h>

namespace orcapublish {

namespace detail {

    inline void convert( const QPicture &pic,
                         orca::QGraphics2dData &orcaPic )
    {
        orcaPic.z = 8;
        orcaPic.isInGlobalCS = true;
        
        orcaPic.qpicture.resize( pic.size() );
        memcpy( &(orcaPic.qpicture[0]), pic.data(), pic.size() );
    }
}

//!
//! @author Alex Brooks
//!
class QGraphics2dPublisher : public hydropublish::QGraphics2dPublisher
{
public: 

    QGraphics2dPublisher( const orcaice::Context &context, const std::string &name )
        : impl_(new orcaifaceimpl::QGraphics2dImpl(context,name))
        {
        }

    inline void init()
        { 
            impl_->initInterface(); 
            QPicture emptyPic;
            localSetAndSend( emptyPic );
        }
    inline void localSetAndSend( const QPicture &picture )
        { 
            detail::convert( picture, orcaQGraphics2dData_ );
            impl_->localSetAndSend( orcaQGraphics2dData_ );
        }

private: 

    orcaifaceimpl::QGraphics2dImplPtr impl_;
    orca::QGraphics2dData orcaQGraphics2dData_;
};

class QGraphics2dPublisherFactory : public hydropublish::QGraphics2dPublisherFactory
{
public:
    QGraphics2dPublisherFactory( const orcaice::Context &context )
        : context_(context)
        {}

    hydropublish::QGraphics2dPublisherPtr createPublisher( const std::string &name )
        { return hydropublish::QGraphics2dPublisherPtr(new QGraphics2dPublisher( context_, name )); }

private:

    orcaice::Context context_;
};

}

#endif
