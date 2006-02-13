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
// **********************************************************************
//
// Copyright (c) 2003-2005 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <Ice/Ice.h>

#include "helloservice_i.h"

#include "platform2d_i.h"

#include <orca/configutils.h>
#include <orca/objutils.h>

using namespace std;
using namespace orca;

extern "C"
{

//
// Factory function
//
HELLO_API IceBox::Service*
create(Ice::CommunicatorPtr communicator)
{
    return new HelloServiceI;
}

}

HelloServiceI::HelloServiceI()
{
}

HelloServiceI::~HelloServiceI()
{
}

void HelloServiceI::start(const string& name,
             const Ice::CommunicatorPtr& communicator,
             const Ice::StringSeq& args)
{
    // create the one-and-only component adapter, parse config file to create adapter name
    if ( orcaice::setComponentProperties( communicator, name ) ) {
        exit( EXIT_FAILURE );
    }
    adapter_ = communicator->createObjectAdapter(name);

    // PROVIDED : Position2d
    string topic1 = orcaice::getTopicName( communicator, "Position2d", name );
    Ice::ObjectPrx obj = orcaice::getIceStormPublisher( communicator, topic1 );
    if ( !obj ) {
        exit( EXIT_FAILURE );
    }
    Position2dConsumerPrx position2dConsumer = Position2dConsumerPrx::uncheckedCast(obj);
    // create servant for direct connections
    Ice::ObjectPtr platform2dObj = new Platform2dI;

    // tell adapter about the new servant
    string name1 = orcaice::getPortName( communicator, "Position2d", name );
    adapter_->add( platform2dObj, Ice::stringToIdentity( name1 ) );

    // start processing clients' requests
    adapter_->activate();


    // objects
    orca::Position2dDataPtr posData = new Position2dData;

    orcaice::setSane( posData );

    while ( 1 )
    {
        cout<<"Pushing data to consumer"<<endl;
        position2dConsumer->setData( posData );

        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(1));
    }

    // instead of while(1){ sleep(x); }
    //communicator()->waitForShutdown();
}

void HelloServiceI::stop()
{
    adapter_->deactivate();
}
