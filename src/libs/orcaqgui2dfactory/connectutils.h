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
        catch ( Ice::ConnectionRefusedException &e )
        {
            std::cout<<"TRACE(IceStormListener::paintInitialData): Caught exception: " << e << std::endl;
        }
        catch ( Ice::Exception &e )
        {
            std::cout<<"TRACE(IceStormListener::paintInitialData): Caught some ice exception: " << e << std::endl;
        }
        catch ( std::exception &e )
        {
            std::cout<<"TRACE(IceStormListener::paintInitialData): Caught some std exception: " << e.what() << std::endl;
        }
        catch ( ... )
        {
            std::cout<<"TRACE(IceStormListener::paintInitialData): Caught some other exception" << std::endl;
        }
    }

}

#endif
