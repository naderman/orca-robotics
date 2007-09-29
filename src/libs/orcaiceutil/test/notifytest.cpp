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
#include <orcaiceutil/notify.h>

using namespace std;

class TestNotifyHandler : public orcaiceutil::NotifyHandler<double>
{
public:
    virtual void handleData( const double& obj )
    {
        copy_=obj;
    };

    double copy_;
};

int main(int argc, char * argv[])
{
    orcaiceutil::Notify<double> notify;
    double data = 2.0;
    
    orcaiceutil::NotifyHandler<double>* emptyHandler = 0;
    TestNotifyHandler testHandler;

    cout<<"testing set() ... ";
    // call set on an empty stomach
    try
    {
        notify.set( data );
        cout<<"failed. empty notify handler, should've caught exception"<<endl;
        return EXIT_FAILURE;
    }
    catch ( const orcaiceutil::Exception & )
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
    catch ( const orcaiceutil::Exception & )
    {
        cout<<"failed. shouldn't have caught exception"<<endl;
        return EXIT_FAILURE;
    }
    if ( data != testHandler.copy_ ) {
        cout<<"failed. expecting an exact copy of the data."<<endl;
        cout<<"\tin="<<data<<" out="<<testHandler.copy_<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;
    
    return EXIT_SUCCESS;
}
