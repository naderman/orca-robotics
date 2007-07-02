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

#include <orcaice/ptrbuffer.h>
#include <orca/rangescanner2d.h>

using namespace std;

int main(int argc, char * argv[])
{
    orcaice::PtrBuffer<orca::RangeScanner2dDataPtr> buffer;
    orca::RangeScanner2dDataPtr data = new orca::RangeScanner2dData;

    cout<<"testing get() ... ";
    // call get on an empty stomach
    try
    {
        buffer.get( data );
        cout<<"failed. empty buffer, should've caught exception"<<endl;
        return EXIT_FAILURE;
    }
    catch ( const orcaice::Exception & )
    {
        ; // ok
    }
    cout<<"ok"<<endl;
    
    cout<<"testing getAndPop()... ";
    try
    {
        buffer.getAndPop( data );
        cout<<"failed. empty buffer, should've caught exception"<<endl;
        return EXIT_FAILURE;
    }
    catch ( const orcaice::Exception & )
    {
        ; // ok
    }
    cout<<"ok"<<endl;

    cout<<"testing getNext() ... ";
    if ( buffer.getNext( data, 50 )==0 ) {
        cout<<"failed. not expecting anybody setting the proxy"<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;

    cout<<"testing getAndPopNext() ... ";
    if ( buffer.getAndPopNext( data, 50 )==0 ) {
        cout<<"failed. not expecting anybody setting the proxy"<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;

    cout<<"testing isEmpty() and size() ... ";
    if ( !buffer.isEmpty() || buffer.size()!=0 ) {
        cout<<"failed. expecting an empty buffer."<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;

    cout<<"testing push() with circular buffer ... ";
    for ( int i=0; i<3; ++i ) {
        buffer.push( data );
    }
    cout<<"ok"<<endl;

    cout<<"testing isEmpty() and size() ... ";
    if ( buffer.isEmpty() || buffer.size()!=-1 ) {
        cout<<"failed. expecting a buffer of size -1."<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;

    cout<<"testing get() ... ";
    try
    {
        buffer.get( data );
    }
    catch ( const orcaice::Exception & )
    {
        cout<<"failed. should be a non-empty buffer."<<endl;
        return EXIT_FAILURE;
    }
    if ( buffer.isEmpty() || buffer.size()!=1 ) {
        cout<<"failed. expecting a buffer of size 1."<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;
    
    cout<<"testing getAndPop()... ";
    try
    {
        buffer.getAndPop( data );
    }
    catch ( const orcaice::Exception & )
    {
        cout<<"failed. should be a non-empty buffer."<<endl;
        return EXIT_FAILURE;
    }
    if ( !buffer.isEmpty() || buffer.size()!=0 ) {
        cout<<"failed. expecting an empty buffer."<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;
    
    cout<<"testing configure() with orcaice::BufferTypeCircular ... ";
    buffer.configure( 300, orcaice::BufferTypeCircular );
    for ( int i=0; i<400; ++i ) {
        buffer.push( data );
    }
    if ( buffer.isEmpty() || buffer.size()!=300 ) {
        cout<<"failed. expecting a buffer of size 300."<<endl;
        return EXIT_FAILURE;
    }
    // todo: test where the last data actually went.
    cout<<"ok"<<endl;

    cout<<"testing purge()... ";
    buffer.purge();
    if ( !buffer.isEmpty() || buffer.size()!=0 ) {
        cout<<"failed. expecting an empty buffer."<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;

    cout<<"testing configure() with orcaice::BufferTypeQueue ... ";
    buffer.configure( 300, orcaice::BufferTypeQueue );
    for ( int i=0; i<400; ++i ) {
        buffer.push( data );
    }
    if ( buffer.isEmpty() || buffer.size()!=300 ) {
        cout<<"failed. expecting a buffer of size 300."<<endl;
        return EXIT_FAILURE;
    }
    // todo: test where the last data actually went.
    cout<<"ok"<<endl;

    cout<<"testing getNext() ... ";
    if ( buffer.getNext( data, 50 )!=0 ) {
        cout<<"failed. expected to get data"<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;

    cout<<"testing getAndPopNext() ... ";
    if ( buffer.getAndPopNext( data, 50 )!=0 ) {
        cout<<"failed. expected to get data"<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;

    cout<<"testing pop() ... ";
    for ( int i=0; i<400; ++i ) {
        buffer.pop();
    }
    if ( !buffer.isEmpty() || buffer.size()!=0 ) {
        cout<<"failed. expecting an empty buffer."<<endl;
        return EXIT_FAILURE;
    }
    // todo: test where the last data actually went.
    cout<<"ok"<<endl;
    
    
    return EXIT_SUCCESS;
}
