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
#include <orcaice/notify.h>
// #include <orcaobj/orcaobj.h>

using namespace std;

class TestNotifyHandler : public orcaice::NotifyHandler<orca::Frame2d>
{
public:
    virtual void handleData( const orca::Frame2d & obj )
    {
        copy_=obj;
    };

    orca::Frame2d copy_;
};

int main(int argc, char * argv[])
{
    orcaice::Notify<orca::Frame2d> notify;
    orca::Frame2d data;
    data.p.x = 10.0;
    data.p.y = 20.0;
    data.o = 30.0;
    
    orcaice::NotifyHandler<orca::Frame2d>* emptyHandler = 0;
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
    if ( data != testHandler.copy_ ) {
        cout<<"failed. expecting an exact copy of the data."<<endl;
        cout<<"\tin\t"<<data.p.x<<" "<<data.p.y<<" "<<data.o<<endl;
        cout<<"\tout\t"<<testHandler.copy_.p.x<<" "<<testHandler.copy_.p.y<<" "<<testHandler.copy_.o<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;
    
    return EXIT_SUCCESS;
}
