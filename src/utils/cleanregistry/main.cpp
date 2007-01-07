/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <IceGrid/Admin.h>
#include <orcaice/orcaice.h>
#include <orcacm/orcacm.h>
#include <orcamisc/orcamisc.h>  // for connectionToRemoteAddress()

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
    : orcaice::Component( "CleanRegistry", orcaice::NoStandardInterfaces  )
{
}

void
Component::start()
{
    // we provide no interfaces, so we don't activate the adapter

    //cout<<"default locator (refresh) :"<<context().communicator()->getDefaultLocator()->ice_toString()<<endl;
    
    std::string locatorString = context().communicator()->getDefaultLocator()->ice_toString();
    
    Ice::ObjectPrx adminProxy = context().communicator()->stringToProxy(
            orcamisc::stringToIceGridInstanceName(locatorString)+"/Admin");
    
    try
    {
        adminProxy->ice_ping();
        std::string adminAddress = orcamisc::connectionToRemoteAddress( adminProxy->ice_getConnection()->toString() );
        cout<<"Ping successful: "<<adminAddress<<endl;
    }
    catch ( const Ice::Exception & e )
    {
        // nothing else to do
        context().communicator()->shutdown();
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
        
    for ( unsigned int i=0; i<list.size(); ++i ) {
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
    context().communicator()->shutdown();
}

//
// Build the component into a stand-alone application
//
int
main(int argc, char * argv[])
{
    Component component;
    orcaice::Application app( component );
    return app.main(argc, argv);
}
