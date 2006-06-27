/*
 *  Orca Project: Components for robotics.
 *
 *  Copyright (C) 2004-2006
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef ORCA2_ORCAMON_GENERIC_SUBSCRIBER_H
#define ORCA2_ORCAMON_GENERIC_SUBSCRIBER_H

#include <iostream>

#include <orcaice/orcaice.h>
#include <orcaice/printingconsumerI.h>

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
            context.tracer()->error( "interface type mismatch. Quitting..." );
            context.communicator()->destroy();
        }
        catch ( const orcaice::NetworkException & e )
        {
            context.tracer()->error( "failed to connect. Will try again after 3 seconds." );
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(3));
        }
        // NOTE: if the user hits ctrl-C, we'll get CommunicatorDestroyedException
        //       which will be caught by Ice::Application and cause exit.
        --count;
    }

    // Get the data once
    try
    {
        context.tracer()->info( "subscriber: Trying to get one data object as a test" );
        objectPrx->getData();
//         context.tracer()->print( orcaice::toString( objectPrx->getData() ) );
    }
    catch ( const orca::OrcaException & e ) 
    {
        std::cout<<e.what<<std::endl;
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
    Ice::ObjectPtr consumer = new orcaice::PrintingConsumerI<ConsumerType,ObjectPtrType>;

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
            context.tracer()->error( "failed to subscribe for data updates. Will try again after 3 seconds." );
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(3));
        }
        --count;
    }
}

#endif
