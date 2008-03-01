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

#include <orcaice/application.h>
#include <orcaice/component.h>

#include <orcaice/orcaice.h>
#include <orcalog/orcalog.h>

using namespace std;

class TestAutoLogger : public orcalog::AutoLogger
{
public:
    TestAutoLogger()
        {}
    virtual ~TestAutoLogger() {}
    virtual void init( const orcalog::LogWriterInfo &logWriterInfo,
                       orcalog::MasterFileWriter &masterFileWriter ) {}
    virtual void startLogging() {}
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
    orcalog::LogWriterInfo info( context() );
    info.interfaceType = "type0";
    info.interfaceTag  = "Tag0";
    info.comment       = "comment0";
    info.format        = "format0";
    info.filename      = "file0";
    TestAutoLogger t1;
    t1.init( info, masterFileWriter );
    info.interfaceType = "type1";
    info.interfaceTag  = "Tag1";
    info.comment       = "comment1";
    info.format        = "format1";
    info.filename      = "file1";
    TestAutoLogger t2;
    t2.init( info, masterFileWriter );

    //
    // Fake a few instances of arrival of data
    //
    int i0=0, index0=0;
    masterFileWriter.notifyOfLogfileAddition( i0, index0, orcaice::getNow() );
    IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(1));
    masterFileWriter.notifyOfLogfileAddition( 0, 1, orcaice::getNow() );
    IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(1));
    masterFileWriter.notifyOfLogfileAddition( 0, 1, orcaice::getNow() );
    IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(1));
    masterFileWriter.notifyOfLogfileAddition( 0, 1, orcaice::getNow() );
    IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(1));
    int id1=1, index1=0;
    masterFileWriter.notifyOfLogfileAddition( id1, index1, orcaice::getNow() );
    IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(1));
    masterFileWriter.notifyOfLogfileAddition( 1, 1, orcaice::getNow() );
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
    int sec, usec, id, index;
    int ret;
    ret = masterFileReader->getData( sec, usec, id, index );
    if ( ret ) {
        cout<<"failed: expected to find line, ret="<<ret<<endl;
        exit(EXIT_FAILURE);
    }
    if ( id!=i0 || index!=index0 ) {
        cout<<"failed"<<endl<<"wrong data in first line"<<endl
            <<"expected="<<i0<<","<<index0
            <<"got="<<id<<","<<index<<endl;
        exit(EXIT_FAILURE);
    }
    cout<<"ok"<<endl;
    int initSec=sec, initUsec=usec;

    cout<<"testing getDataAtOrAfterTime() forward to good line ... ";
    int goodSec = sec+3, goodUsec=usec+900000;
    ret = masterFileReader->getDataAtOrAfterTime( sec, usec, id, index,  goodSec, goodUsec );
    if ( ret ) {
        cout<<"failed: ret="<<ret<<endl;
        exit(EXIT_FAILURE);
    }
    if ( id!=id1 || index!=index1 ) {
        cout<<"failed"<<endl<<"wrong data in sought line"<<endl
            <<"expected="<<id1<<","<<index1
            <<" got="<<id<<","<<index<<endl;
        exit(EXIT_FAILURE);
    }
    cout<<"ok"<<endl;

    cout<<"testing placeCursorBeforeTime() for good line ... ";
    masterFileReader->placeCursorBeforeTime( goodSec, goodUsec );
    ret = masterFileReader->getData( sec, usec, id, index );
    if ( ret ) {
        cout<<"failed: ret="<<ret<<endl;
        exit(EXIT_FAILURE);
    }
    if ( orcalog::iceUtilTime(sec,usec) >= orcalog::iceUtilTime(goodSec,goodUsec) )
    {
        cout<<"failed"<<endl
            <<"sec=    "<<sec<<    ", usec=    "<<usec<<endl
            <<"goodSec="<<goodSec<<", goodUsec="<<goodUsec<<endl;
        exit(EXIT_FAILURE);
    }
    if ( orcalog::iceUtilTime(sec,usec) == orcalog::iceUtilTime(initSec,initUsec) )
    {
        cout<<"failed"<<endl
            <<"sec=    "<<sec<<    ", usec=    "<<usec<<endl
            <<"initSec="<<initSec<<", initUsec="<<initUsec<<endl;
        exit(EXIT_FAILURE);
    }
    cout<<"ok"<<endl;

    cout<<"testing getDataAtOrAfterTime() to line which is after the end of log  ... ";
    ret = masterFileReader->getDataAtOrAfterTime( sec, usec, id, index,  sec+1000 );
    if ( !ret ) {
        cout<<"failed: expected not to find line, ret="<<ret<<endl
            <<"got="<<sec<<","<<usec<<","<<id<<","<<index<<endl;
        exit(EXIT_FAILURE);
    }
    cout<<"ok"<<endl;

    cout<<"testing getDataAtOrAfterTime() backward to good line from after end ... ";
    ret = masterFileReader->getDataAtOrAfterTime( sec, usec, id, index,  goodSec, goodUsec );
    if ( ret ) {
        cout<<"failed: ret="<<ret<<endl;
        exit(EXIT_FAILURE);
    }
    if ( id!=id1 || index!=index1 ) {
        cout<<"failed"<<endl<<"wrong data in sought line"<<endl
            <<"expected="<<id1<<","<<index1
            <<" got="<<id<<","<<index<<endl;
        exit(EXIT_FAILURE);
    }
    cout<<"ok"<<endl;

    cout<<"testing rewindToStart() ... ";
    masterFileReader->rewindToStart();
    ret = masterFileReader->getData( sec, usec, id, index );
    if ( ret ) {
        cout<<"failed: expected to find line, ret="<<ret<<endl;
        exit(EXIT_FAILURE);
    }
    if ( id!=i0 || index!=index0 ) {
        cout<<"failed"<<endl<<"wrong data in first line"<<endl
            <<"expected="<<i0<<","<<index0
            <<"got="<<id<<","<<index<<endl;
        exit(EXIT_FAILURE);
    }
    cout<<"ok"<<endl;

    cout<<"Testing arbitrary access ... ";
    int rSec, rUsec, rId, rIndex;
    ret = masterFileReader->getData( rSec, rUsec, rId, rIndex );
    assert( !ret );
    ret = masterFileReader->getData( sec, usec, id, index );
    assert( !ret );
    masterFileReader->getDataAtOrAfterTime( sec, usec, id, index, rSec, rUsec );
    if ( sec != rSec     ||
         usec != rUsec   ||
         id != rId       ||
         index != rIndex )
    {
        cout<<"failed."<<endl;
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
