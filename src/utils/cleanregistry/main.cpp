/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>
#include <orcacm/orcacm.h>
#include <IceGrid/Registry.h>

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

private:
    void purgeObjects( const IceGrid::AdminPrx& admin );
    void purgeAdapters( const IceGrid::AdminPrx& admin );
};


Component::Component() : 
    orcaice::Component( "CleanRegistry", orcaice::NoStandardInterfaces  )
{
}

void
Component::purgeObjects( const IceGrid::AdminPrx& admin )
{
    std::string locatorString = context().communicator()->getDefaultLocator()->ice_toString();

    orcacm::RegistryHomeData regData;
    bool tryToPing = true;
    regData = orcacm::getRegistryHomeData( context(), locatorString, tryToPing );

    if ( !regData.isReachable ) {
        context().tracer().error( "Registry '"+locatorString+"' is unreachable" );
        // nothing else to do
        context().communicator()->shutdown();
    }

    cout<<"retrieved list of "<<regData.homes.size()<<" home objects"<<endl;

    int aliveCount = 0;
    int deadCount = 0;
        
    for ( unsigned int i=0; i<regData.homes.size(); ++i ) 
    {    
        // ping each component's Home interface
        if ( regData.homes[i].isReachable ) {
            ++aliveCount;
        }
        // make sure it's an Orca component, it's platform compName should be non-empty
        else {
            try
            {
                cout<<"about to remove Home object with ID '"<<regData.homes[i].proxy->ice_toString()<<"'"<<endl;
                admin->removeObject( regData.homes[i].proxy->ice_getIdentity() );
            }
            catch ( const IceGrid::ObjectNotRegisteredException & e )
            {
                cout<<e<<endl;
            }
            catch ( const IceGrid::DeploymentException & e )
            {
                cout<<e<<endl;
            }
            
            ++deadCount;
        }
    }
    
    cout<<"registered  : "<<regData.homes.size()<<endl;
    cout<<"alive       : "<<aliveCount<<endl;
    cout<<"removed     : "<<deadCount<<endl;
}


void
Component::purgeAdapters( const IceGrid::AdminPrx& admin )
{
    //
    // get adapter adaptList
    //
    Ice::StringSeq adaptList = admin->getAllAdapterIds();
    cout<<"retrieved list of "<<adaptList.size()<<" adapters"<<endl;

    orca::FQComponentName compName;
    std::string homeIdentity;

    int aliveCount = 0;
    int deadCount = 0;
        
    for ( unsigned int i=0; i<adaptList.size(); ++i ) 
    {
        compName = orcaice::toComponentName( adaptList[i] );
        homeIdentity = orcaice::toHomeIdentity( compName );
        
        // ping each component's Home interface
        if ( orcacm::pingObject( context(), homeIdentity ) ) {
            ++aliveCount;
        }
        // make sure it's an Orca component, it's platform compName should be non-empty
        else if ( !compName.platform.empty() ) {
            try
            {
                cout<<"about to remove adapter with ID '"<<adaptList[i]<<"'"<<endl;
                admin->removeAdapter( adaptList[i] );
            }
            catch ( const IceGrid::AdapterNotExistException & e )
            {
                cout<<e<<endl;
            }
            catch ( const IceGrid::DeploymentException & e )
            {
                cout<<e<<endl;
            }
            
            ++deadCount;
        }
        else {
            cout<<"will not clean "<<compName.component<<endl;
        }
    }
    
    cout<<"registered  : "<<adaptList.size()<<endl;
    cout<<"alive       : "<<aliveCount<<endl;
    cout<<"removed     : "<<deadCount<<endl;
    cout<<"non-Orca    : "<<adaptList.size()-aliveCount-deadCount<<endl;
}

void
Component::start()
{
    // we provide no interfaces, so we don't activate the adapter
    cout<<"default locator (refresh) :"<<context().communicator()->getDefaultLocator()->ice_toString()<<endl;
    
    std::string locatorString = context().communicator()->getDefaultLocator()->ice_toString();
    std::string instanceName = orcacm::stringToIceGridInstanceName(locatorString);

    Ice::ObjectPrx base = context().communicator()->stringToProxy( instanceName + "/Registry" );

//     Ice::ObjectPrx adminProxy = context().communicator()->stringToProxy(
//             orcacm::stringToIceGridInstanceName(locatorString)+"/Admin");
    IceGrid::AdminPrx adminProxy;

    try
    {
        // Open an admin session with the registry
        IceGrid::RegistryPrx registry = IceGrid::RegistryPrx::checkedCast(base);
        // This assumes no access control
        std::string username = "no-access-control-assumed";
        std::string password = "no-access-control-assumed";
        IceGrid::AdminSessionPrx adminSession = registry->createAdminSession( username, password );
        
        adminProxy = adminSession->getAdmin();
        adminProxy->ice_ping();
        std::string adminAddress = orcacm::connectionToRemoteAddress( adminProxy->ice_getConnection()->toString() );
        cout<<"Ping successful: "<<adminAddress<<endl;
    }
    catch ( const Ice::Exception & e )
    {
        context().tracer().error( "Registry '"+locatorString+"' is unreachable" );
        // nothing else to do
        context().communicator()->shutdown();
    }
    
//    IceGrid::AdminPrx admin = IceGrid::AdminPrx::checkedCast( adminProxy );
    
    //
    // Well-known objects
    //
    purgeObjects( adminProxy );

    //
    // Adapters
    //
    purgeAdapters( adminProxy );

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
