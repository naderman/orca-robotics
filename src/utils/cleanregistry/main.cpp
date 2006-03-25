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

#include <iostream>
#include <IceGrid/Admin.h>
#include <orcaice/orcaice.h>
#include <orcacm/orcacm.h>

#include <orcaice/application.h>
#include <orcaice/component.h>

using namespace std;

class Component : public orcaice::Component
{
public:
    Component();
    virtual ~Component() {};

    // component interface
    virtual void start();
    virtual void stop() {};
};


Component::Component()
    : orcaice::Component( "CleanRegistry", orcaice::HomeInterface  )
{
}

void
Component::start()
{
    //cout<<"default locator (refresh) :"<<context().communicator()->getDefaultLocator()->ice_toString()<<endl;
    
    std::string locatorString = context().communicator()->getDefaultLocator()->ice_toString();
    
    Ice::ObjectPrx adminProxy = context().communicator()->stringToProxy(
            orcaice::stringToIceGridInstanceName(locatorString)+"/Admin");
    
    try
    {
        adminProxy->ice_ping();
        std::string adminAddress = orcaice::connectionToRemoteAddress( adminProxy->ice_connection()->toString() );
        cout<<"Ping successful: "<<adminAddress<<endl;
    }
    catch ( const Ice::Exception & e )
    {
        // nothing else to do
        communicator()->shutdown();
    }
    
    IceGrid::AdminPrx admin = IceGrid::AdminPrx::checkedCast( adminProxy );
    
    //
    // get adapter list
    //
    Ice::StringSeq list = admin->getAllAdapterIds();
    cout<<"retrieved a list of "<<list.size()<<" adapters"<<endl;

    orca::FQComponentName name;

    int reachCount = 0;
    int cleanCount = 0;
        
    for ( uint i=0; i<list.size(); ++i ) {
        name = orcaice::toComponentName( list[i] );
        
        // ping each component's Home interface
        if ( orcacm::pingComponent( context(), list[i] ) ) {
            ++reachCount;
        }
        // make sure it's an Orca component, it's platform name should be non-empty
        else if ( !name.platform.empty() ) {
            try
            {
                cout<<"about to remove adapter with ID '"<<list[i]<<"'"<<endl;
                admin->removeAdapter( list[i] );
            }
            catch ( const IceGrid::AdapterNotExistException & e )
            {
                cout<<e<<endl;
            }
            catch ( const IceGrid::DeploymentException & e )
            {
                cout<<e<<endl;
            }
            
            ++cleanCount;
        }
        else {
            cout<<"will not clean "<<name.component<<endl;
        }
    }
    
    cout<<"registered  : "<<list.size()<<endl;
    cout<<"active      : "<<reachCount<<endl;
    cout<<"removed     : "<<cleanCount<<endl;
    cout<<"non-Orca    : "<<list.size()-reachCount-cleanCount<<endl;

    // we are done
    communicator()->shutdown();
}

//
// Build the component into a stand-alone application
//
int
main(int argc, char * argv[])
{
    Component component;
    orcaice::Application app( argc, argv, component );
    return app.main(argc, argv);
}
