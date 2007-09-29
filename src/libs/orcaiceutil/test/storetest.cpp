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
#include <orcaiceutil/store.h>

using namespace std;

int main(int argc, char * argv[])
{
    orcaiceutil::Store<double> store;
    double data = 20.0;
    double copy = -1.0;

    cout<<"testing get() ... ";
    // call get on an empty stomach
    try
    {
        store.get( data );
        cout<<"failed. empty store, should've caught exception"<<endl;
        return EXIT_FAILURE;
    }
    catch ( const orcaiceutil::Exception & )
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
    catch ( const orcaiceutil::Exception & )
    {
        cout<<"failed. should be a non-empty store."<<endl;
        return EXIT_FAILURE;
    }
    if ( data!=copy )
    {
        cout<<"failed. expecting an exact copy of the data."<<endl;
        cout<<"\tin="<<data<<" out="<<copy<<endl;
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
