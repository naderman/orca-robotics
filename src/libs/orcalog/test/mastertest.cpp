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

#include <orcaice/application.h>
#include <orcaice/component.h>

#include <orcaice/orcaice.h>
#include <orcalog/orcalog.h>

using namespace std;

class TestLogger : public orcalog::Logger
{
public:
    TestLogger( orcalog::MasterFileWriter &masterFileWriter,
                const std::string         &interfaceType, 
                const std::string         &comment,
                const std::string         &format,
                const std::string         &filename,
                const orcaice::Context    &context ) :
        orcalog::Logger( masterFileWriter, interfaceType, comment, format, filename, context ) {};
    virtual ~TestLogger() {};
    virtual void init() {};
};

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
    orcalog::MasterFileWriter masterFileWriter( masterFilename.c_str(), context() );
    // fake properties
    context().properties()->setProperty(  "MasterTest.Requires.Tag0.Proxy", "bullshit" );
    context().properties()->setProperty(  "MasterTest.Requires.Tag1.Proxy", "horseshit" );
    // add a few logs
    new TestLogger( masterFileWriter, "type0", "comment0", "format0", "file0", context() );
    new TestLogger( masterFileWriter, "type1", "comment1", "format1", "file1", context() );

    int l0=0, d0=0;
    masterFileWriter.notifyOfLogfileAddition( l0, d0 );
    IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(1));
    masterFileWriter.notifyOfLogfileAddition( 0, 1 );
    IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(1));
    masterFileWriter.notifyOfLogfileAddition( 0, 1 );
    IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(1));
    masterFileWriter.notifyOfLogfileAddition( 0, 1 );
    IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(1));
    int l1=1, d1=0;
    masterFileWriter.notifyOfLogfileAddition( l1, d1 );
    IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(1));
    masterFileWriter.notifyOfLogfileAddition( 1, 1 );
    cout<<"ok"<<endl;

    cout<<"testing LogMaster() ... ";
    orcalog::ReplayMaster* playMaster = new orcalog::ReplayMaster( masterFilename.c_str(), context() );

    // get info on all logs from the master file
    std::vector<std::string> filenames;
    std::vector<std::string> interfaceTypes;
    std::vector<std::string> formats;
    std::vector<bool> enableds;
    // this may throw and it will kill us
    playMaster->getLogs( filenames, interfaceTypes, formats, enableds );
    if ( filenames.size() != (unsigned int)masterFileWriter.loggerCount() ) {
        cout<<"failed"<<endl<<"log count expected="<<masterFileWriter.loggerCount()<<" got="<<filenames.size()<<endl;
        exit(EXIT_FAILURE);
    }
    cout<<"ok"<<endl;

    cout<<"testing getData() ... ";
    int s, u, l, d;
    int ret;
    ret = playMaster->getData( s, u, l, d );
    if ( ret ) {
        cout<<"failed: expected to find line, ret="<<ret<<endl;
        exit(EXIT_FAILURE);
    }
    if ( l!=l0 || d!=d0 ) {
        cout<<"failed"<<endl<<"wrong data in first line"<<endl
            <<"expected="<<l0<<","<<d0
            <<"got="<<l<<","<<d<<endl;
        exit(EXIT_FAILURE);
    }
    cout<<"ok"<<endl;
    
    cout<<"testing FF getData() to good line ... ";
    ret = playMaster->getData( s, u, l, d,  s+3, u+900000 );
    if ( ret ) {
        cout<<"failed: ret="<<ret<<endl;
        exit(EXIT_FAILURE);
    }
    if ( l!=l1 || d!=d1 ) {
        cout<<"failed"<<endl<<"wrong data in sought line"<<endl
            <<"expected="<<l1<<","<<d1
            <<" got="<<l<<","<<d<<endl;
        exit(EXIT_FAILURE);
    }
    cout<<"ok"<<endl;

    cout<<"testing FF getData() to line which is after the end of log  ... ";
    ret = playMaster->getData( s, u, l, d,  s+1000 );
    if ( !ret ) {
        cout<<"failed: expected not to find line, ret="<<ret<<endl
            <<"got="<<s<<","<<u<<","<<l<<","<<d<<endl;
        exit(EXIT_FAILURE);
    }
    cout<<"ok"<<endl;

    cout<<"testing seekDataStart() ... ";
    ret = playMaster->seekDataStart();
    if ( ret ) {
        cout<<"failed: expected find data, ret="<<ret<<endl;
        exit(EXIT_FAILURE);
    }
    ret = playMaster->getData( s, u, l, d );
    if ( ret ) {
        cout<<"failed: expected to find line, ret="<<ret<<endl;
        exit(EXIT_FAILURE);
    }
    if ( l!=l0 || d!=d0 ) {
        cout<<"failed"<<endl<<"wrong data in first line"<<endl
            <<"expected="<<l0<<","<<d0
            <<"got="<<l<<","<<d<<endl;
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
