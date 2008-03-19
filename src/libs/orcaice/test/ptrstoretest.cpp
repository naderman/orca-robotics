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

#include <orca/rangescanner2d.h>
#include <orcaice/ptrstore.h>
// #include <orcaobj/orcaobj.h>
#include <gbxsickacfr/gbxutilacfr/mathdefs.h>

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

int main(int argc, char * argv[])
{
    orcaice::PtrStore<orca::RangeScanner2dDataPtr> store;
    orca::RangeScanner2dDataPtr data = new orca::RangeScanner2dData;
    setSane( data );
    orca::RangeScanner2dDataPtr copy = new orca::RangeScanner2dData;

    cout<<"testing get() ... ";
    // call get on an empty stomach
    try
    {
        store.get( data );
        cout<<"failed. empty store, should've caught exception"<<endl;
        return EXIT_FAILURE;
    }
    catch ( const gbxsickacfr::gbxutilacfr::Exception & )
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
    catch ( const gbxsickacfr::gbxutilacfr::Exception & )
    {
        cout<<"failed. should be a non-empty store."<<endl;
        return EXIT_FAILURE;
    }
    // apparently structure have operator== but classes don't
    // (on the other hand, classes have default constructors but structures don't)
    if ( data->timeStamp != copy->timeStamp
         || data->maxRange != copy->maxRange
         || data->maxRange != copy->maxRange
         || data->fieldOfView != copy->fieldOfView
         || data->ranges != copy->ranges )
    {
        cout<<"failed. expecting an exact copy of the data."<<endl;
        cout<<"\tin\ttime="<<data->timeStamp.seconds<<":"<<data->timeStamp.useconds<<" data="<<data<<endl;
        cout<<"\tin\ttime="<<copy->timeStamp.seconds<<":"<<copy->timeStamp.useconds<<" data="<<copy<<endl;
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
