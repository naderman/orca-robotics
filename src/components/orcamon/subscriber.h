/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ORCAMON_GENERIC_SUBSCRIBER_H
#define ORCA2_ORCAMON_GENERIC_SUBSCRIBER_H

#include <iostream>

#include <orcaice/orcaice.h>
#include <orcaifaceimpl/printingconsumerI.h>

template<class ObjectPrxType, class ComsumerPrxType, class ConsumerType, class ObjectPtrType>
void attach( const orcaice::Context & context, const std::string & proxyString )
{
    std::cout<<"generic attach()"<<std::endl;
    // Connect directly to the interface
    ObjectPrxType objectPrx;
    int count = 3;
    while( count )
    {
        try
        {
            orcaice::connectToInterfaceWithString<ObjectPrxType>( context, objectPrx, proxyString );
            break;
        }
        catch ( const orcaice::TypeMismatchException & e )
        {
            std::stringstream ss;
            ss << "interface type mismatch: " << e.what() << ".  Quitting...";
            context.tracer()->error( ss.str() );
            context.communicator()->destroy();
        }
        catch ( const orcaice::NetworkException & e )
        {
            std::stringstream ss;
            ss << "failed to connect: " << e.what() << ".  Will try again after 3 seconds.";
            context.tracer()->error( ss.str() );
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(3));
        }
        // NOTE: if the user hits ctrl-C, we'll get CommunicatorDestroyedException
        //       which will be caught by Ice::Application and cause exit.
        --count;
    }

    // Get the data once
    try
    {
        context.tracer()->info( "subscriber: Trying to get one data object as a test: " );
        objectPrx->getData();
        context.tracer()->print( orcaice::toString( objectPrx->getData() ) );
    }
    catch ( const orca::OrcaException & e ) 
    {
        std::cout<<e<<": "<<e.what<<std::endl;
    }
    catch ( const Ice::Exception & e ) 
    {
        std::cout<<e<<std::endl;
    }
    catch ( ... )
    {
        context.tracer()->error( "caught something." );
        exit(1);
    }
//     catch ( const orca::DataNotExistException & e )
//     {
//         std::cout<<e<<std::endl;
//         abort;
//         context.tracer()->error( "data is not in the buffer yet. Will subscribe anyway." );
//     }
//     catch ( const orca::HardwareFailedException & e )
//     {
//         context.tracer()->error( "hardware failure reported when getting data. Will subscribe anyway." );
//     }

    // create a callback object to recieve scans
//     Ice::ObjectPtr consumer = new ConsumerIType;
    Ice::ObjectPtr consumer = new orcaifaceimpl::PrintingConsumerI<ConsumerType,ObjectPtrType>;

    ComsumerPrxType callbackPrx = orcaice::createConsumerInterface<ComsumerPrxType>( context, consumer );
    //
    // ENABLE NETWORK CONNECTIONS
    //
    // do activation directly on the adapter
    context.adapter()->activate();
    
    //
    // Subscribe for data
    //
    count = 3;
    while ( count )
    {
        try
        {
            objectPrx->subscribe( callbackPrx );
            break;
        }
        catch ( const orca::SubscriptionFailedException & e )
        {
            std::stringstream ss;
            ss << "failed to subscribe: " << e.what << ".  Will try again after 3 seconds.";
            context.tracer()->error( ss.str() );
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(3));
        }
        --count;
    }
}

#endif
