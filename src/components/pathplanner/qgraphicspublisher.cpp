#ifdef QT4_FOUND
#include "qgraphicspublisher.h"
#include <iostream>

using namespace std;

namespace pathplanner {

namespace {

    void convert( const QPicture &pic,
                  orca::QGraphics2dData &orcaPic )
    {
        orcaPic.z = 4;
        orcaPic.isInGlobalCS = true;
        
        orcaPic.qpicture.resize( pic.size() );
        memcpy( &(orcaPic.qpicture[0]), pic.data(), pic.size() );
    }

}

QGraphicsPublisher::QGraphicsPublisher( const orcaice::Context &context,
                                        const std::string &name )
    : qGraphics2dImpl_(new orcaifaceimpl::QGraphics2dImpl(context,name))
{
}

void
QGraphicsPublisher::init()
{
    qGraphics2dImpl_->initInterface();
    QPicture emptyPic;
    localSetAndSend( emptyPic );
}

void
QGraphicsPublisher::localSetAndSend( const QPicture &pic )
{
    convert( pic, orcaPic_ );
    qGraphics2dImpl_->localSetAndSend( orcaPic_ );
}

}

#endif
