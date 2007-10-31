/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_IFACE_IMPL_UTIL_H
#define ORCA_IFACE_IMPL_UTIL_H

#include <orcaice/context.h>
#include <orcaice/connectutils.h>
#include <sstream>
#include <iostream>
#include <IceStorm/IceStorm.h>

namespace orcaifaceimpl {

    /*!
        Tries to push to IceStorm. If fails tries to reconnect to IceStorm once. If reconnects
        successfully, pushes the data, if not, ignores the problem until the next time. 
        Catches all exceptions. 
    */
    template<class ConsumerPrxType,
             class DataType>
    void
    tryPushToIceStormWithReconnect( orcaice::Context   &context,
                                    ConsumerPrxType    &consumerPrx,
                                    const DataType     &data,
                                    IceStorm::TopicPrx &topicPrx,
                                    const std::string  &interfaceName,
                                    const std::string  &topicName )
    {
        // check that communicator still exists
        if ( !context.communicator() ) {
            return;
        }
    
        try {
            consumerPrx->setData( data );
        }
        catch ( Ice::CommunicatorDestroyedException & )
        {
            // If we see this, we're obviously shutting down.  Don't bitch about anything.
        }
        catch ( Ice::Exception &e )
        {
            // This could happen if IceStorm dies.
            // If we're running in an IceBox and the IceBox is shutting down, 
            // this is expected (our co-located IceStorm is obviously going down).
            std::stringstream ss; ss << interfaceName << ": Failed push to IceStorm: " << e;
            context.tracer()->warning( ss.str() );

            // If IceStorm just re-started for some reason though, we want to try to re-connect
            try
            {
                context.tracer()->info( "Re-connecting to IceStorm..." );
                topicPrx = orcaice::connectToTopicWithString<ConsumerPrxType>
                    ( context, consumerPrx, topicName );
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

    // Remove the ptr from the adapter if possible.
    // If not, fail silently.
    void
    tryRemovePtr( orcaice::Context &context,
                  Ice::ObjectPtr &ptr );


    // Convenience function for tag->name conversion
    // (may throw ConfigFileException)
    std::string getInterfaceNameFromTag( const orcaice::Context &context,
                                         const std::string      &interfaceTag );
    
    // Convenience function for interfaceName->topicName conversion
    std::string getTopicNameFromInterfaceName( const orcaice::Context &context,
                                               const std::string      &interfaceTag );
}

#endif
