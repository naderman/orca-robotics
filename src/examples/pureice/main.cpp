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
#include <sstream>
#include <cmath> // for M_PI

#include <Ice/Ice.h>

#include <orca/laser.h>

using namespace std;

// this function is a copy from libOrcaIce (sysutils.cpp)
std::string 
getHostname()
{
    char hostname[256];
   
#ifndef WIN32 
    int ret = gethostname(hostname,256);
    if ( ret==0 ) {
        return std::string( hostname );
    }
    else {
        return std::string("localhost");
    }
#else
    cout<<"getHostname() is not implemented under Windows"<<endl;
    exit(1);
    //! @todo implement getHostname in win
    return std::string("localhost");
#endif
}

// this function is from libOrcaObjects (stringutils.cpp)
std::string 
toString( const orca::RangeScannerDataPtr & obj)
{
    std::ostringstream s;
//     s << toString(obj->timeStamp)
    s << " RangeScannerData [" << obj->ranges.size() << " elements]: " << endl;
    s << "\tstartAngle: " << obj->startAngle * 180.0/M_PI << "deg" << endl;
    s << "\tangleIncrement: " << obj->angleIncrement * 180.0/M_PI << "deg" << endl;

    return s.str();
}

//
// This client code is based on the example from
// Chapter 7 Developing a File System Client in C++
// in the Ice User Manual.
//
int
main(int argc, char* argv[])
{
    int status = 0;
    Ice::CommunicatorPtr ic;
    try 
    {
        // Create a communicator
        //
        ic = Ice::initialize(argc, argv);
        cout<<"Communicator initialized..."<<endl;

        // Create a proxy for the laser
        //
        // we'll use indirect proxy here to connect to the sicklaser component
        // setup according to the Quick Start tutorial of Orca2
        // see: orca-robotics.sf.net/orca_doc_quickstart.html
        Ice::ObjectPrx base = ic->stringToProxy( "laser@"+getHostname()+"/sicklaser" );
        if (!base) {
            throw "Could not create proxy";
        }
        cout<<"Base proxy created..."<<endl;

        base->ice_ping();

        // Down-cast the proxy to a Laser proxy
        //
        orca::LaserPrx laser = orca::LaserPrx::checkedCast(base);
        if (!laser) {
            throw "Invalid proxy";
        }
        cout<<"Derived proxy created..."<<endl;

        // get one laser scan and print it out
        //
        orca::RangeScannerDataPtr data = new orca::LaserData;
        data = laser->getData();

        cout<<"Received laser scan:"<<endl;
        cout<<toString( data )<<endl;
    } 
    catch (const Ice::Exception& ex) 
    {
        cerr << ex << endl;
        status = 1;
    }
    catch (const char* msg) 
    {
        cerr << msg << endl;
        status = 1;
    }

    // Clean up
    //
    if (ic)
    ic->destroy();

    return status;
}
