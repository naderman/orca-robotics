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

#include <orca/rangescanner2d.h>
#include <orcaice/ptrnotify.h>
// #include <orcaobj/orcaobj.h>
#include <gbxutilacfr/mathdefs.h>

// Random doubles and integers
#define RDOUBLE (rand()*M_PI)
#define RINT    (rand())
#define RCHAR   ((char)rand())

using namespace std;

// copied over from orcaob/initutils.h to cut dependency
void 
setSane( const orca::RangeScanner2dDataPtr& obj, int numberOfSamples=361 )
{
//     orca::Time t;
//     setSane( t );
    obj->timeStamp.seconds = 12345;
    obj->timeStamp.seconds = 6789;

    obj->maxRange = 80.0;
    obj->fieldOfView = M_PI;
    obj->startAngle = -(obj->fieldOfView/2.0);

    for ( int i=0; i<numberOfSamples; ++i ) {
        obj->ranges.push_back( fmod((float)RINT,80) );
    }
}

// copied over from orcaob/stringutils.h to cut dependency
std::string toString( const orca::RangeScanner2dDataPtr& obj, int skip=-1  )
{
    std::ostringstream s;
//     s << toString(obj->timeStamp)
    s << obj->timeStamp.seconds<<":"<<obj->timeStamp.useconds
      << " RangeScanner2dData [" << obj->ranges.size() << " elements]: " << endl;

    s << "minRange=" << obj->minRange << "m" << endl;
    s << "maxRange=" << obj->maxRange << "m" << endl;
    s << "fieldOfView=" << obj->fieldOfView * 180.0/M_PI << "deg" << endl;
    s << "startAngle=" << obj->startAngle * 180.0/M_PI << "deg" << endl;

    if ( skip > -1 ) 
    {
        s << ": (";
        for ( unsigned int i=0; i < obj->ranges.size(); ++i ) {
            s << obj->ranges[i] << " ";
            i = i + skip;
        }
        s << ")";
        s << endl;
    }
    return s.str();
}


class TestNotifyHandler : public gbxiceutilacfr::NotifyHandler<orca::RangeScanner2dDataPtr>
{
public:
    TestNotifyHandler()
        : copy_( new orca::RangeScanner2dData )
    {};
    
    virtual void handleData( const orca::RangeScanner2dDataPtr & obj )
    {
        *copy_=*obj;
    };

    orca::RangeScanner2dDataPtr copy_;
};

int 
main(int argc, char * argv[])
{
    orcaice::PtrNotify<orca::RangeScanner2dDataPtr> notify;
    orca::RangeScanner2dDataPtr data = new orca::RangeScanner2dData;
    setSane( data );
    
    gbxiceutilacfr::NotifyHandler<orca::RangeScanner2dDataPtr>* emptyHandler = 0;
    TestNotifyHandler testHandler;

    cout<<"testing set() ... ";
    // call set on an empty stomach
    try
    {
        notify.set( data );
        cout<<"failed. empty notify handler, should've caught exception"<<endl;
        return EXIT_FAILURE;
    }
    catch ( const gbxutilacfr::Exception & )
    {
        ; // ok
    }
    cout<<"ok"<<endl;

    cout<<"testing hasNotifyHandler() ... ";
    if ( notify.hasNotifyHandler()!=0 ) {
        cout<<"failed. not expecting to have a handler"<<endl;
        return EXIT_FAILURE;
    }
    notify.setNotifyHandler( emptyHandler );
    if ( notify.hasNotifyHandler()!=0 ) {
        cout<<"failed. still not expecting to have a handler"<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;

    cout<<"testing setNotifyHandler() ... ";
    notify.setNotifyHandler( &testHandler );
    if ( notify.hasNotifyHandler()==0 ) {
        cout<<"failed. expecting to have a handler"<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;

    cout<<"testing set() ... ";
    try
    {
        notify.set( data );
    }
    catch ( const gbxutilacfr::Exception & )
    {
        cout<<"failed. shouldn't have caught exception"<<endl;
        return EXIT_FAILURE;
    }
    if ( data->timeStamp != testHandler.copy_->timeStamp
         || data->minRange != testHandler.copy_->minRange
         || data->maxRange != testHandler.copy_->maxRange
         || data->fieldOfView != testHandler.copy_->fieldOfView
         || data->ranges != testHandler.copy_->ranges )
    {
        cout<<"failed. expecting an exact copy of the data."<<endl;
        cout<<"\tin\ttime="<<data->timeStamp.seconds<<":"<<data->timeStamp.useconds<<" data="<<data<<endl;
        cout<<"\tin\ttime="<<testHandler.copy_->timeStamp.seconds<<":"<<testHandler.copy_->timeStamp.useconds
                <<" data="<<testHandler.copy_<<endl;
        return EXIT_FAILURE;
    }
    
    cout<<"ok"<<endl;
    
    return EXIT_SUCCESS;
}
