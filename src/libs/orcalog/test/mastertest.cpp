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

class TestAutoLogger : public orcalog::AutoLogger
{
public:
    TestAutoLogger( const orcalog::LogWriterInfo &logWriterInfo )
        {}
    virtual ~TestAutoLogger() {}
    virtual void initAndStartLogging() {}
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
    cout<<"testing MasterFileWriter() ... ";

    //
    // create master file
    //
    string masterFilename = "testmaster.txt";
    orcalog::MasterFileWriter masterFileWriter( masterFilename.c_str(), context() );

    //
    // fake properties
    //
    context().properties()->setProperty(  "MasterTest.Requires.Tag0.Proxy", "bullshit" );
    context().properties()->setProperty(  "MasterTest.Requires.Tag1.Proxy", "horseshit" );

    //
    // add a few logs
    //
    orcalog::LogWriterInfo info( masterFileWriter, context() );
    info.interfaceType = "type0";
    info.interfaceTag  = "Tag0";
    info.comment       = "comment0";
    info.format        = "format0";
    info.filename      = "file0";
    new TestAutoLogger( info );
    info.interfaceType = "type1";
    info.interfaceTag  = "Tag1";
    info.comment       = "comment1";
    info.format        = "format1";
    info.filename      = "file1";
    new TestAutoLogger( info );

    //
    // Fake a few instances of arrival of data
    //
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

    cout<<"testing replaying ... ";
    orcalog::MasterFileReader *masterFileReader = new orcalog::MasterFileReader( masterFilename, context() );

    // get info on all logs from the master file
    std::vector<std::string> filenames;
    std::vector<std::string> interfaceTypes;
    std::vector<std::string> formats;
    std::vector<bool> enableds;
    // this may throw and it will kill us
    masterFileReader->getLogs( filenames, interfaceTypes, formats, enableds );
    if ( filenames.size() != (unsigned int)masterFileWriter.loggerCount() ) {
        cout<<"failed"<<endl<<"log count expected="<<masterFileWriter.loggerCount()<<" got="<<filenames.size()<<endl;
        exit(EXIT_FAILURE);
    }
    cout<<"ok"<<endl;

    cout<<"testing getData() ... ";
    int s, u, l, d;
    int ret;
    ret = masterFileReader->getData( s, u, l, d );
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
    ret = masterFileReader->getDataAfterTime( s, u, l, d,  s+3, u+900000 );
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
    ret = masterFileReader->getDataAfterTime( s, u, l, d,  s+1000 );
    if ( !ret ) {
        cout<<"failed: expected not to find line, ret="<<ret<<endl
            <<"got="<<s<<","<<u<<","<<l<<","<<d<<endl;
        exit(EXIT_FAILURE);
    }
    cout<<"ok"<<endl;

    cout<<"testing seekDataStart() ... ";
    ret = masterFileReader->rewindToStart();
    if ( ret ) {
        cout<<"failed: expected find data, ret="<<ret<<endl;
        exit(EXIT_FAILURE);
    }
    ret = masterFileReader->getData( s, u, l, d );
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
