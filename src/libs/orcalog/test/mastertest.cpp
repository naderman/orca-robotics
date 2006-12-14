/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>

#include <orcaice/application.h>
#include <orcaice/component.h>

#include <orcaice/orcaice.h>
#include <orcalog/orcalog.h>

using namespace std;

class TestComponent : public orcaice::Component
{
public:
    TestComponent();
    virtual ~TestComponent();

    // component interface
    virtual void start();
    virtual void stop();
};


TestComponent::TestComponent()
    : orcaice::Component( "MasterTest" )
{
}

TestComponent::~TestComponent()
{
}

void TestComponent::start()
{
    cout<<"testing LogMaster() ... ";
    string masterFilename = "testmaster.txt";
    // create logMaster file
    orcalog::LogMaster* logMaster = new orcalog::LogMaster( masterFilename.c_str(), context() );
    // add a few logs
    int id;
    id = logMaster->addLog( "file0", "type0", "format0", "proxy0" );
    id = logMaster->addLog( "file1", "type1", "format1", "proxy1" );
    int logCount = id+1;

    int s0=24, u0=555, l0=0, d0=0;
    logMaster->addData( s0, u0, l0, d0 );
    logMaster->addData( 24, 888000, 0, 1 );
    int s1=25, u1=333, l1=1, d1=0;
    logMaster->addData( 25, 333, 1, 0 );
    logMaster->addData( 25, 333000, 1, 1 );
    cout<<"ok"<<endl;

    cout<<"testing LogMaster() ... ";
    orcalog::ReplayMaster* playMaster = new orcalog::ReplayMaster( masterFilename.c_str(), context() );

    // get info on all logs from the master file
    std::vector<std::string> filenames;
    std::vector<std::string> interfaceTypes;
    std::vector<std::string> formats;
    // this may throw and it will kill us
    playMaster->getLogs( filenames, interfaceTypes, formats );
    if ( filenames.size() != logCount ) {
        cout<<"failed"<<endl<<"log count expected="<<logCount<<" got="<<filenames.size()<<endl;
        exit(EXIT_FAILURE);
    }
    cout<<"ok"<<endl;

    cout<<"testing getData() ... ";
    int s, u, l, d;
    playMaster->getData( s, u, l, d );
    if ( s!=s0 || u!=u0 || l!=l0 || d!=d0 ) {
        cout<<"failed"<<endl<<"wrong data in first line"<<endl
            <<"expected="<<s0<<","<<u0<<","<<l0<<","<<d0
            <<"expected="<<s<<","<<u<<","<<l<<","<<d<<endl;
        exit(EXIT_FAILURE);
    }
    cout<<"ok"<<endl;
    
    cout<<"testing seekData() ... ";
    playMaster->seekData( s, u, l, d, 25 );
    if ( s!=s1 || u!=u1 || l!=l1 || d!=d1 ) {
        cout<<"failed"<<endl<<"wrong data in sought line"<<endl
            <<"expected="<<s1<<","<<u1<<","<<l1<<","<<d1
            <<"expected="<<s<<","<<u<<","<<l<<","<<d<<endl;
        exit(EXIT_FAILURE);
    }
    cout<<"ok"<<endl;
    

    // NOTE: cannot call communicator()->destroy() from here
    // because they'll be caught by Ice::Application and show up as failed ctest.
    exit(EXIT_SUCCESS);
}

void TestComponent::stop()
{
    // nothing to do
}

//
// Build the component into a stand-alone application
//
int main(int argc, char * argv[])
{
    TestComponent component;
    orcaice::Application app( component );
    return app.main(argc, argv);
}
