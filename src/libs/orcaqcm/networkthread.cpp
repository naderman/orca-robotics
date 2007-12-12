/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>

#include <orcaice/orcaice.h>
#include <orcacm/orcacm.h>

#include "networkthread.h"
#include "calls.h"

using namespace std;
using namespace orcaqcm;

NetworkThread::NetworkThread( const orcaice::Context & context )
        : context_(context)
{
}

NetworkThread::~NetworkThread()
{
}

void
NetworkThread::getComponentInfo()
{
    try
    {
        cout<<"adding registry job to the queue..."<<endl;
        this->add( new GetComponentsCall( context_, context_.communicator()->getDefaultLocator()->ice_toString(), &homePipe_ ) );
    }
    catch( const Ice::Exception & e )
    {
        cerr<<e<<endl;
    }
}

void
NetworkThread::getProperties( const std::string & proxy )
{

}

/*
// this function is called by the UserHandler
std::map<std::string,std::string>
NetworkThread::getProperties( const std::string & homeProxyString )
{
    orca::ComponentProperties props;
    try
    {
        Ice::ObjectPrx ohome = context_.communicator()->stringToProxy( homeProxyString );
        orca::HomePrx home = orca::HomePrx::checkedCast(ohome);
                    
        props = home->getProperties();

        std::cout<<"got "<<props.size()<<" properties"<<std::endl;
        return props;
    }
    catch( const Ice::Exception & e )
    {
        std::cout << "failed to lookup properties for "<< homeProxyString << std::endl;
        std::map<std::string,std::string> dummy;
        return dummy;
    }
}
*/
