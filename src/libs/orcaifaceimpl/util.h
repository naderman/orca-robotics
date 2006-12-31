#ifndef ORCA_IFACE_IMPL_UTIL_H
#define ORCA_IFACE_IMPL_UTIL_H

#include <orcaice/context.h>
#include <sstream>

namespace orcaifaceimpl {

    template<class ConsumerPrxType,
             class DataType>
    void
    tryPushToIceStormWithReconnect( orcaice::Context   &context,
                                    ConsumerPrxType    &consumerPrx,
                                    const DataType     &data,
                                    IceStorm::TopicPrx &topicPrx,
                                    const std::string  &ifaceTag )
    {
        try {
            consumerPrx->setData( data );
        }
        catch ( Ice::Exception &e )
        {
            // This could happen if IceStorm dies.
            // If we're running in an IceBox and the IceBox is shutting down, 
            // this is expected (our co-located IceStorm is obviously going down).
            std::stringstream ss; ss << ifaceTag << ": Failed push to IceStorm: " << e;
            context.tracer()->warning( ss.str() );

            // If IceStorm just re-started for some reason though, we want to try to re-connect
            try
            {
                context.tracer()->info( "Re-connecting to IceStorm..." );
                topicPrx = orcaice::connectToTopicWithTag<ConsumerPrxType>
                    ( context, consumerPrx, ifaceTag );
                context.tracer()->info( "Re-connected to IceStorm." );

                // try again to push that bit of info
                consumerPrx->setData( data );
            }
            catch ( ... )
            {
                // ignore it -- we'll try again next push.
                context.tracer()->info( "Re-connection to IceStorm failed." );
            }
        }

    }
}

#endif
