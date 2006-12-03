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

#include <orca/position2d.h>
#include <orcaice/ptrnotify.h>
#include <orcaobj/orcaobj.h>

using namespace std;

class TestNotifyHandler : public orcaice::NotifyHandler<orca::Position2dDataPtr>
{
public:
    TestNotifyHandler()
        : copy_( new orca::Position2dData )
    {};
    
    virtual void handleData( const orca::Position2dDataPtr & obj )
    {
        *copy_=*obj;
    };

    orca::Position2dDataPtr copy_;
};

int 
main(int argc, char * argv[])
{
    orcaice::PtrNotify<orca::Position2dDataPtr> notify;
    orca::Position2dDataPtr data = new orca::Position2dData;
    orcaice::setSane( data );
    
    orcaice::NotifyHandler<orca::Position2dDataPtr>* emptyHandler = 0;
    TestNotifyHandler testHandler;

    cout<<"testing set() ... ";
    // call set on an empty stomach
    try
    {
        notify.set( data );
        cout<<"failed. empty notify handler, should've caught exception"<<endl;
        return EXIT_FAILURE;
    }
    catch ( const orcaice::Exception & )
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
    catch ( const orcaice::Exception & )
    {
        cout<<"failed. shouldn't have caught exception"<<endl;
        return EXIT_FAILURE;
    }
    if ( data->timeStamp != testHandler.copy_->timeStamp
         || data->pose != testHandler.copy_->pose
         || data->motion != testHandler.copy_->motion
         || data->stalled != testHandler.copy_->stalled )
    {
        cout<<"failed. expecting an exact copy of the data."<<endl;
        cout<<"\tin\t"<<orcaice::toString(data->timeStamp)<<" "<<data<<endl;
        cout<<"\tout\t"<<orcaice::toString(testHandler.copy_->timeStamp)<<" "<<testHandler.copy_<<endl;
        return EXIT_FAILURE;
    }
    
    cout<<"ok"<<endl;
    
    return EXIT_SUCCESS;
}
