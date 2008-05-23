/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>
#include <orcacm/orcacm.h>
#include <orcaprobe/orcaprobe.h>
#include <orcaifacestring/properties.h>

#include "propertiesprobe.h"

using namespace std;
using namespace orcaprobefactory;

PropertiesProbe::PropertiesProbe( const orca::FQInterfaceName & name, orcaprobe::AbstractDisplay & display,
                                const orcaice::Context & context )
    : InterfaceProbe(name,display,context)
{
    id_ = "::orca::Properties";
    
    addOperation( "getData" );
    addOperation( "setData" );
    addOperation( "subscribe" );
    addOperation( "unsubscribe" );
    
    consumer_ = new orcaifaceimpl::PrintingPropertiesConsumerImpl( context,1000,1 );
}
    
PropertiesProbe::~PropertiesProbe()
{
    consumer_->destroy();
}

int 
PropertiesProbe::loadOperationEvent( const int index, orcacm::OperationData & data )
{
    switch ( index )
    {
    case orcaprobe::UserIndex :
        return loadGetData( data );
    case orcaprobe::UserIndex+1 :
        return loadSetData( data );
    case orcaprobe::UserIndex+2 :
        return loadSubscribe( data );
    case orcaprobe::UserIndex+3 :
        return loadUnsubscribe( data );
    }
    return 1;
}

int 
PropertiesProbe::loadGetData( orcacm::OperationData& data )
{
    orca::PropertiesPrx derivedPrx = orca::PropertiesPrx::checkedCast(prx_);
    orcaprobe::reportResult( data, "data", ifacestring::toString( derivedPrx->getData() ) );
    return 0;
}

int 
PropertiesProbe::loadSetData( orcacm::OperationData & data )
{
    try
    {
        orca::PropertiesPrx derivedPrx = orca::PropertiesPrx::checkedCast(prx_);

        // AlexB: Not sure why this is necessary...
        string dummy;
        std::getline( cin, dummy );

        cout << endl;
        cout << "Enter property to set: ";
        string prop;
        std::getline( cin, prop );
        // cout<<"TRACE(propertiesprobe.cpp): read prop: '" << prop << "'" << endl;

        cout << "Enter property value: ";
        string val;
        std::getline( cin, val );
        // cout<<"TRACE(propertiesprobe.cpp): read val: '" << val << "'" << endl;

        std::map<string,string> props;
        props[prop] = val;
        orca::PropertiesData propData;
        propData.properties = props;

        derivedPrx->setData( propData );
    }
    catch( const orca::DataNotExistException & e )
    {
        orcaprobe::reportException( data, "data is not ready on the remote interface" );
    }
    catch( const orca::HardwareFailedException & e )
    {
        orcaprobe::reportException( data, "remote hardware failure" );
    }
    catch( const Ice::Exception & e )
    {
        stringstream ss;
        ss<<e<<endl;
        orcaprobe::reportException( data, ss.str() );
    }
    return 0;
}

int 
PropertiesProbe::loadSubscribe( orcacm::OperationData& data )
{
    consumer_->subscribeWithString( orcaice::toString(name_) );
    orcaprobe::reportSubscribed( data, consumer_->consumerPrx()->ice_toString() );
    return 0;
}

int 
PropertiesProbe::loadUnsubscribe( orcacm::OperationData& data )
{
    consumer_->unsubscribeWithString( orcaice::toString(name_) );
    orcaprobe::reportUnsubscribed( data );
    return 0;
}
