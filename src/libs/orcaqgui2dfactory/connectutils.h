#ifndef CONNECTUTIL_H
#define CONNECTUTIL_H

#include <orcaice/context.h>
#include <string>
#include <orcaqgui2dfactory/connectutils.h>

namespace orcaqgui2d {
    
    template< class ProxyType,
              class PainterType >
    void
    paintInitialData( orcaice::Context      &context,
                      const std::string     &proxyString,
                      PainterType           &painter )
    {
        try {
            // Connect directly to the interface
            ProxyType prx;
            orcaice::connectToInterfaceWithString( context, prx, proxyString );
            painter.setData( prx->getData() );
        }
        catch ( Ice::Exception &e )
        {
            std::cout<<"TRACE(connectutils::paintInitialData): exception: " << e << std::endl;
            throw;
        }
        catch ( std::exception &e )
        {
            std::cout<<"TRACE(connectutils::paintInitialData): exception: " << e.what() << std::endl;
            throw;
        }
        catch ( ... )
        {
            std::cout<<"TRACE(connectutils::paintInitialData): unknown exception" << std::endl;
            throw;
        }
    }

}

#endif
