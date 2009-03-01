/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>

#include <orcaice/application.h>
#include <orcaice/component.h>

#include <orcaice/orcaice.h>
#include <orcaice/icegridutils.h>
#include <orca/home.h>
#include <orca/status.h>
#include <orca/tracer.h>
#include <orca/power.h>

using namespace std;

class TestComponent : public orcaice::Component
{
public:
    TestComponent() : orcaice::Component( "IceGridTest", orcaice::AllStandardInterfaces ) {};
    virtual void start();
};

void 
TestComponent::start()
{
    // LOCAL FUNCTIONS

    cout<<"testing toAdminIdentity() and the reverse toComponentName() ... ";
    {
        orca::FQComponentName in, out;
        in.platform = "papa";
        in.component = "charlie";
        Ice::Identity id = orcaice::toAdminIdentity( in );
        out = orcaice::toComponentName( id );
        if ( in != out ) {
            cout<<"failed: in="<<orcaice::toString(in)<<" out="<<orcaice::toString(out)<<endl;
            exit(EXIT_FAILURE);
        }

        Ice::Identity dummyId;
        dummyId.name="dummy";
        Ice::ObjectPrx objPrx = context().adapter()->createProxy( dummyId );
        // without a facet
        objPrx = objPrx->ice_identity( id );
        out = orcaice::toComponentName( objPrx );
        if ( in != out ) {
            cout<<"failed: in="<<orcaice::toString(in)<<" out="<<orcaice::toString(out)<<endl;
            exit(EXIT_FAILURE);
        }
        // with a facet
        objPrx = objPrx->ice_facet( "Crap" );
        out = orcaice::toComponentName( objPrx );
        if ( in != out ) {
            cout<<"failed: in="<<orcaice::toString(in)<<" out="<<orcaice::toString(out)<<endl;
            exit(EXIT_FAILURE);
        }
    }
    cout<<"ok"<<endl;
    
    cout<<"testing toAdminFacet() ... ";
    {
        orca::FQComponentName comp;
        comp.platform = "papa";
        comp.component = "charlie";
        if ( orcaice::toAdminFacet( comp, "::Ice::Process" ) != "Process" ) {
            cout<<"failed"<<endl<<"for Process got :"<<orcaice::toAdminFacet( comp, "::Ice::Process" )<<endl;
            exit(EXIT_FAILURE);
        }
        if ( orcaice::toAdminFacet( comp, "::Ice::PropertiesAdmin" ) != "Properties" ) {
            cout<<"failed"<<endl<<"for PropertiesAdmin got :"<<orcaice::toAdminFacet( comp, "::Ice::PropertiesAdmin" )<<endl;
            exit(EXIT_FAILURE);
        }
        if ( orcaice::toAdminFacet( comp, "::orca::Home" ) != "papa.charlie.Home" ) {
            cout<<"failed"<<endl<<"for Home got :"<<orcaice::toAdminFacet( comp, "::orca::Home" )<<endl;
            exit(EXIT_FAILURE);
        }
        if ( orcaice::toAdminFacet( comp, "::orca::Status" ) != "papa.charlie.Status" ) {
            cout<<"failed"<<endl<<"for Status got :"<<orcaice::toAdminFacet( comp, "::orca::Status" )<<endl;
            exit(EXIT_FAILURE);
        }
        if ( orcaice::toAdminFacet( comp, "::orca::Tracer" ) != "papa.charlie.Tracer" ) {
            cout<<"failed"<<endl<<"for Tracer got :"<<orcaice::toAdminFacet( comp, "::orca::Tracer" )<<endl;
            exit(EXIT_FAILURE);
        }
        try {
            orcaice::toAdminFacet( comp, "::orca::Something" );
            cout<<"failed"<<endl<<"should've thrown."<<endl;
            exit(EXIT_FAILURE);
        }
        catch ( const std::exception& )
        {}
    }
    cout<<"ok"<<endl;


    // NETWORKED FUNCTIONS

    // if set to FALSE, will return success when registry is not available
    const bool failWithoutRegistry = false;

    cout<<"testing isRegistryReachable() ... ";
    {
        bool isUp = orcaice::isRegistryReachable( context() );
        if ( !isUp ) {
            if ( failWithoutRegistry ) {
                cout<<"failed: could not reach registry"<<endl;
                exit(EXIT_FAILURE);
            }
            else {
                cout<<"could not connect to registry."<<endl;
                cout<<"The test is configured not to fail."<<endl;
                exit(EXIT_SUCCESS);
            }
        }
    }
    cout<<"ok"<<endl;

    // from now on, we know that the registry exists, so we don't tolerate exceptions

    // sleep a bit to allow Home registration
    IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(2));

    cout<<"testing isAdminInterfaceReachable() with Home and platform=local ... ";
    {
        orca::HomePrx objectPrx;
        orca::FQComponentName fqName;
        fqName.platform = "local";
        fqName.component = "icegridtest";
        string diagnostic;
        if ( !orcaice::isAdminInterfaceReachable( context(), fqName, "::orca::Home", diagnostic ) ) {
            cout<<"Failed to ping Home: "<<diagnostic<<endl;
            exit(EXIT_FAILURE);
        }
    }
    cout<<"ok"<<endl;

    cout<<"testing isAdminInterfaceReachable() with Status ... ";
    {
        orca::StatusPrx objectPrx;
        string diagnostic;
        if ( !orcaice::isAdminInterfaceReachable( context(), context().name(), "::orca::Status", diagnostic ) ) {
            cout<<"Failed to ping Status: "<<diagnostic<<endl;
            exit(EXIT_FAILURE);
        }
    }
    cout<<"ok"<<endl;

    cout<<"testing isAdminInterfaceReachable() with Tracer ... ";
    {
        orca::TracerPrx objectPrx;
        string diagnostic;
        if ( !orcaice::isAdminInterfaceReachable( context(), context().name(), "::orca::Tracer", diagnostic ) ) {
            cout<<"Failed to ping Tracer: "<<diagnostic<<endl;
            exit(EXIT_FAILURE);
        }
    }
    cout<<"ok"<<endl;

    cout<<"testing isAdminInterfaceReachable() with Process ... ";
    {
        Ice::ProcessPrx objectPrx;
        string diagnostic;
        // Process should be ON
        if ( !orcaice::isAdminInterfaceReachable( context(), context().name(), "::Ice::Process", diagnostic ) ) {
            cout<<"Failed to ping Process: "<<diagnostic<<endl;
            exit(EXIT_FAILURE);
        }
    }
    cout<<"ok"<<endl;

    cout<<"testing isAdminInterfaceReachable() with Properties ... ";
    {
        Ice::PropertiesAdminPrx objectPrx;
        string diagnostic;
        // Properties should be OFF
        if ( orcaice::isAdminInterfaceReachable( context(), context().name(), "::Ice::PropertiesAdmin", diagnostic ) ) {
            cout<<"Should not be able to ping Properties: "<<diagnostic<<endl;
            exit(EXIT_FAILURE);
        }
    }
    cout<<"ok"<<endl;

    cout<<"testing connectToAdminInterface() with Home and platform=local ... ";
    {
        orca::HomePrx objectPrx;
        orca::FQComponentName fqName;
        fqName.platform = "local";
        fqName.component = "icegridtest";
        try {
            orcaice::connectToAdminInterface<orca::Home,orca::HomePrx>( context(), objectPrx, fqName );
        }
        catch ( const std::exception& e ) {
            cout<<"Failed to connect to Home: "<<e.what()<<endl;
            exit(EXIT_FAILURE);
        }
        // try to use it
        try {
            orca::HomeData data = objectPrx->getData();
            if ( data.comp.name.component != context().name().component ) {
                cout<<"got wrong comp name="<<data.comp.name.component<<endl;
                exit(EXIT_FAILURE);
            }
        }
        catch ( const std::exception& e ) {
            cout<<"Failed to get status: "<<e.what()<<endl;
            exit(EXIT_FAILURE);
        }
    }
    cout<<"ok"<<endl;

    cout<<"testing connectToAdminInterface() with Status ... ";
    {
        orca::StatusPrx objectPrx;
        try {
            orcaice::connectToAdminInterface<orca::Status,orca::StatusPrx>( context(), objectPrx, context().name() );
        }
        catch ( const std::exception& e ) {
            cout<<"Failed to connect to Status: "<<e.what()<<endl;
            exit(EXIT_FAILURE);
        }
        // try to use it
        try {
            orca::StatusData data = objectPrx->getData();
            if ( data.compStatus.name.component != context().name().component ) {
                cout<<"got wrong comp name="<<data.compStatus.name.component<<endl;
                exit(EXIT_FAILURE);
            }
        }
        catch ( const std::exception& e ) {
            cout<<"Failed to get status: "<<e.what()<<endl;
            exit(EXIT_FAILURE);
        }
    }
    cout<<"ok"<<endl;

    cout<<"testing connectToAdminInterface() with Tracer ... ";
    {
        orca::TracerPrx objectPrx;
        try {
            orcaice::connectToAdminInterface<orca::Tracer,orca::TracerPrx>( context(), objectPrx, context().name() );
        }
        catch ( const std::exception& e ) {
            cout<<"Failed to connect to Tracer: "<<e.what()<<endl;
            exit(EXIT_FAILURE);
        }
        // try to use it
        try {
            orca::TracerVerbosityConfig data = objectPrx->getVerbosity();
//             if ( data.compStatus.name.component != context().name().component ) {
//                 cout<<"got wrong comp name="<<data.compStatus.name.component<<endl;
//                 exit(EXIT_FAILURE);
//             }
        }
        catch ( const std::exception& e ) {
            cout<<"Failed to get verbosity: "<<e.what()<<endl;
            exit(EXIT_FAILURE);
        }
    }
    cout<<"ok"<<endl;

    cout<<"testing connectToAdminInterface() with Power ... ";
    {
        orca::PowerPrx objectPrx;
        try {
            orcaice::connectToAdminInterface<orca::Power,orca::PowerPrx>( context(), objectPrx, context().name() );
            cout<<"Should not be able to connect"<<endl;
            exit(EXIT_FAILURE);
        }
        catch ( const std::exception& e ) {
            cout<<"Failed to connect to Power: "<<e.what()<<endl;
        }
    }
    cout<<"ok"<<endl;

    cout<<"testing connectToAdminInterface() with Process ... ";
    {
        Ice::ProcessPrx objectPrx;
        try {
            orcaice::connectToAdminInterface<Ice::Process,Ice::ProcessPrx>( context(), objectPrx, context().name() );
        }
        catch ( const std::exception& e ) {
            cout<<"Failed to connect to Process: "<<e.what()<<endl;
            exit(EXIT_FAILURE);
        }
        // try to use it
        try {
            objectPrx->writeMessage("THIS CRAZY MESSAGE CAME ACCROSS THE WIRE", 1);
            // nothing to test. :(
        }
        catch ( const std::exception& e ) {
            cout<<"Failed to use interface: "<<e.what()<<endl;
            exit(EXIT_FAILURE);
        }
    }
    cout<<"ok"<<endl;

    cout<<"testing connectToAdminInterface() with PropertiesAdminPrx ... ";
    {
        Ice::PropertiesAdminPrx objectPrx;

        // let's test one case with a disabled interface (we setting Orca.Component.EnableProperties=0 in the config file.)
        try {
            orcaice::connectToAdminInterface<Ice::PropertiesAdmin,Ice::PropertiesAdminPrx>( context(), objectPrx, context().name() );
            cout<<"This should not connect"<<endl;
            exit(EXIT_FAILURE);
        }
        catch ( const std::exception& e ) {
            // ok
        }

        // this will work if the interface is enabled
//         try {
//             orcaice::connectToAdminInterface<Ice::PropertiesAdmin,Ice::PropertiesAdminPrx>( context(), objectPrx, context().name() );
//         }
//         catch ( const std::exception& e ) {
//             cout<<"Failed to connect to PropertiesAdmin: "<<e.what()<<endl;
//             exit(EXIT_FAILURE);
//         }
//         // try to use it
//         try {
//             Ice::PropertyDict data = objectPrx->getPropertiesForPrefix( "" );
//             if ( data.empty() ) {
//                 cout<<"got empty prop dict"<<endl;
//                 exit(EXIT_FAILURE);
//             }
//         }
//         catch ( const std::exception& e ) {
//             cout<<"Failed to get properties: "<<e.what()<<endl;
//             exit(EXIT_FAILURE);
//         }
    }
    cout<<"ok"<<endl;

    context().shutdown();
}

int 
main(int argc, char * argv[])
{
    TestComponent component;
    orcaice::Application app( component );
    return app.orcaMain(argc, argv);
}
