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

#include <orca/bros1.h>
#include <orcaice/store.h>

using namespace std;

int main(int argc, char * argv[])
{
    orcaice::Store<orca::Frame2d> store;
    orca::Frame2d data;
    data.p.x = 10.0;
    data.p.y = 20.0;
    data.o = 30.0;
    orca::Frame2d copy;

    cout<<"testing get() ... ";
    // call get on an empty stomach
    try
    {
        store.get( data );
        cout<<"failed. empty store, should've caught exception"<<endl;
        return EXIT_FAILURE;
    }
    catch ( const orcaice::Exception & )
    {
        ; // ok
    }
    cout<<"ok"<<endl;

    cout<<"testing getNext() ... ";
    if ( store.getNext( data, 50 )==0 ) {
        cout<<"failed. not expecting anybody setting the store"<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;
    
    cout<<"testing isEmpty() and isNewData() ... ";
    if ( !store.isEmpty() || store.isNewData() ) {
        cout<<"failed. expecting an empty non-new store."<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;

    cout<<"testing set() ... ";
    for ( int i=0; i<3; ++i ) {
        store.set( data );
    }
    if ( store.isEmpty() || !store.isNewData() ) {
        cout<<"failed. expecting a non-empty new store."<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;

    cout<<"testing get() ... ";
    try
    {
        store.get( copy );
    }
    catch ( const orcaice::Exception & )
    {
        cout<<"failed. should be a non-empty store."<<endl;
        return EXIT_FAILURE;
    }
    // apparently structure have operator== which compares every field
    // but classes don't (classes just test if the smart pointer points
    // to the same locatioin in memory)
    // (on the other hand, classes have default constructors but structures don't)
    if ( data!=copy )
    {
        cout<<"failed. expecting an exact copy of the data."<<endl;
        cout<<"\tin\t"<<data.p.x<<" "<<data.p.y<<" "<<data.o<<endl;
        cout<<"\tout\t"<<copy.p.x<<" "<<copy.p.y<<" "<<copy.o<<endl;
        return EXIT_FAILURE;
    }
    if ( store.isEmpty() || store.isNewData() ) {
        cout<<"failed. expecting a non-empty non-new store."<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;

    cout<<"testing getNext() ... ";
    store.set( data );
    if ( store.getNext( data, 50 )!=0 ) {
        cout<<"failed. expected to get data"<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;
    
    cout<<"testing purge()... ";
    store.purge();
    if ( !store.isEmpty() || store.isNewData() ) {
        cout<<"failed. expecting an empty non-new store."<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;

    
    return EXIT_SUCCESS;
}
