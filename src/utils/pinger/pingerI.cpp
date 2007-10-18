/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <algorithm>
#include <iostream>
#include <cmath>

#include <orca/laserscanner2d.h>
#include <orca/ogmap.h>
#include <orca/camera.h>

#include <orcaice/orcaice.h>

#include "pingerI.h"

using namespace std;

PingerI::PingerI( const orcaice::Context& context, 
                    orca::ReplierPrx & replier, 
                    std::ofstream* logfile, 
                    ping_config_t config ) :
    context_(context),
    replier_(replier),
    logfile_(logfile),
    config_(config)
{
    // preallocate send/receive times
    for ( int i=0; i<config_.count; ++i )
    {
        sends_.push_back( IceUtil::Time() );
        receives_.push_back( IceUtil::Time() );
        ids_.push_back( -1 );       // magic number: if these stay "-1", means object_ lost
    }
}

void 
PingerI::callback(const orca::OrcaObjectPtr& base, const Ice::Current&)
{
    orca::OrcaObjectPtr object = base;
    //orca::Pose2DPtr object = orca::Pose2DPtr::dynamicCast( base );

    // stop the clock
    stopWatch_ = IceUtil::Time::now();

    //std::cout<<"Received reply. x="<<object->x<<std::endl;

    // which object_ is it?
    int id = object->timeStamp.seconds;

    receives_[id] = stopWatch_;

    // extract the sequential ID stored in the seconds field
    ids_[id] = object->timeStamp.seconds;

    // printout here does not affect timing
    std::cout<<"got something back: "<<id<<std::endl;
}

void 
PingerI::takeItBack(const orca::PingReplyData& data, const Ice::Current&)
{
    // stop the clock
    stopWatch_ = IceUtil::Time::now();

    receives_[data.id] = stopWatch_;
    ids_[data.id] = data.id;

    // printout here does not affect timing
    std::cout<<"got something back: "<<data.id<<std::endl;
}

bool 
PingerI::sequencePing( int operationType )
{
    //std::cout<<std::endl<<"object: "<<objectName_<<std::endl;

    // convert from [s] to [us]
    int interval = (int)floor( config_.interval * 1.0e6 );
    
    for ( int j=0; j<6; ++j )
    {
        // create ping object
        orca::PingReplyData data;
        for ( int k=0; k<pow( 10.0, double(j) ); ++k ) {
            data.doubles.push_back( k/33.0 );
        }
        // ID is stored as an int
        int objSize = sizeof(Ice::Int) + data.doubles.size()*sizeof(Ice::Double);

        // create return object
        orca::PingReplyData returnData;

        for ( int i=0; i<config_.count; ++i )
        {
    
            switch ( operationType )
            {
            case 1 : // 1 CLASS-1WAY
            {
                // set object sequential number
                data.id = i;
                // time stamp right before sending
                sends_[i] = IceUtil::Time::now();
                replier_->takeIt( data );
                // receive times are recorded in the PingerI::callback (same host/clock)
                break;
            }
            case 2 :    // 2 CLASS-2WAY
            {
                // set object sequential number
                data.id = i;
                // time stamp right before sending
                sends_[i] = IceUtil::Time::now();
    
                for ( int j=0; j<config_.preload; ++j )
                {
                    returnData = replier_->takeItAndReturn( data );
                }
                // stop the clock
                stopWatch_ = IceUtil::Time::now();
                receives_[returnData.id] = stopWatch_;
                ids_[returnData.id] = returnData.id;
                break;
            }
            } // switch
            // print out here does not affect timing
    //         std::cout<<"sent id:"<<i<<std::endl;
    
            // NOTE: interval (period) is approximate, assume send process happens quickly
            // should be ok for longish intervals.
            // if this is a problem, estimate how much time has passed
    //         std::cout<<"sleeping for "<<interval<<"us"<<std::endl;
            IceUtil::ThreadControl::sleep(IceUtil::Time::microSeconds(interval));
        } // sending the same objects COUNT times

        calcStats( objSize );

    } // diff size objects


    return true;
}

bool 
PingerI::classPing()
{
    // pre-configured object to push, won't touch again except for the serial id
    orca::OrcaObjectPtr base;

    cout<<"Sending CLASSES"<<endl;
    // 2-way ping
    const int pingType = 2;
    int length = 0;
    vector<Ice::Byte> byteData;

    cout<<"sending orca object (time stamp only)"<<endl;
    base = new orca::OrcaObject;
    orca::Time t;
    orcaice::setSane( t );
    base->timeStamp = t;
    // estimate object's size
    {
        Ice::OutputStreamPtr outStreamPtr = Ice::createOutputStream( context_.communicator() );
        orca::ice_writeOrcaObject( outStreamPtr, base );
        outStreamPtr->writePendingObjects();
        // this actually writes to data to the byte vector
        outStreamPtr->finished( byteData );
        length = byteData.size();
    }
    internalPing( base, length, pingType ); 

    cout<<"sending laser"<<endl;
    base = new orca::LaserScanner2dData;
    orca::LaserScanner2dDataPtr scan = orca::LaserScanner2dDataPtr::dynamicCast( base );
    orcaice::setSane( scan );
    // estimate object's size
    {
        Ice::OutputStreamPtr outStreamPtr = Ice::createOutputStream( context_.communicator() );
        orca::ice_writeLaserScanner2dData( outStreamPtr, scan );
        outStreamPtr->writePendingObjects();
        // this actually writes to data to the byte vector
        outStreamPtr->finished( byteData );
        length = byteData.size();
    }
    internalPing( base, length, pingType );

    cout<<"sending BIG laser"<<endl;
    base = new orca::LaserScanner2dData;
    orca::LaserScanner2dDataPtr scan2 = orca::LaserScanner2dDataPtr::dynamicCast( base );
    const int size = 3610;
    for ( int i=0; i<size; ++i ) {
        scan2->ranges.push_back( fmod((float)rand(),80) );
        scan2->intensities.push_back( char(rand()%8) );
    }
    // estimate object's size
    {
        Ice::OutputStreamPtr outStreamPtr = Ice::createOutputStream( context_.communicator() );
        orca::ice_writeLaserScanner2dData( outStreamPtr, scan2 );
        outStreamPtr->writePendingObjects();
        // this actually writes to data to the byte vector
        outStreamPtr->finished( byteData );
        length = byteData.size();
    }
    internalPing( base, length, pingType );
/*
    cout<<"sending ogmap"<<endl;
    base = new orca::OgMapData;
    orca::OgMapDataPtr ogmap = orca::OgMapDataPtr::dynamicCast( base );
    orcaice::setSane( ogmap );
    // estimate object's size
    {
        Ice::OutputStreamPtr outStreamPtr = Ice::createOutputStream( context_.communicator() );
        orca::ice_writeOgMapData( outStreamPtr, ogmap );
        outStreamPtr->writePendingObjects();
        // this actually writes to data to the byte vector
        outStreamPtr->finished( byteData );
        length = byteData.size();
    }
    internalPing( base, length, pingType );

    cout<<"sending camera"<<endl;
    base = new orca::CameraData;
    orca::CameraDataPtr image = orca::CameraDataPtr::dynamicCast( base );
    orcaice::setSane( image );
    // estimate object's size
    {
        Ice::OutputStreamPtr outStreamPtr = Ice::createOutputStream( context_.communicator() );
        orca::ice_writeCameraData( outStreamPtr, image );
        outStreamPtr->writePendingObjects();
        // this actually writes to data to the byte vector
        outStreamPtr->finished( byteData );
        length = byteData.size();
    }
    internalPing( base, length, pingType );
*/  
    return true;
}

bool 
PingerI::icePing()
{
    // we are not using this, needed for compatibility
    orca::OrcaObjectPtr base;

    cout<<"Sending ICEPING"<<endl;

    internalPing( base, 1, 0 );
    
    return true;
}

// 0 ICE-PING
// 1 CLASS-1WAY
// 2 CLASS-2WAY
bool 
PingerI::internalPing( orca::OrcaObjectPtr & object, const int objSize, int pingType )
{
    //std::cout<<std::endl<<"object: "<<objectName_<<std::endl;

    // convert from [s] to [us]
    int interval = (int)floor( config_.interval * 1.0e6 );
    
    for ( int i=0; i<config_.count; ++i )
    {

        switch ( pingType )
        {
        case 0 :   // 0 ICE-PING
        {
            // time stamp right before sending
            sends_[i] = IceUtil::Time::now();

            for ( int j=0; j<config_.preload; ++j )
            {
                replier_->ice_ping();
            }
            // stop the clock
            receives_[i] = IceUtil::Time::now();
            // whatever, as long as it's not -1
            ids_[i] = 9;
            break;
        }
        case 1 : // 1 CLASS-1WAY
        {
            // by convetion, set the seconds field of the time stamp to the sequential number
            object->timeStamp.seconds = i;
            // time stamp right before sending
            sends_[i] = IceUtil::Time::now();
            replier_->ping1( object );
            // receive times are recorded in the PingerI::callback (same host/clock)
            break;
        }
        case 2 :    // 2 CLASS-2WAY
        {
            // by convetion, set the seconds field of the time stamp to the sequential number
//             cout<<"sending object "<<i<<endl;
            object->timeStamp.seconds = i;
            // time stamp right before sending
            sends_[i] = IceUtil::Time::now();

            for ( int j=0; j<config_.preload; ++j )
            {
                object = replier_->ping2( object );
            }
            // stop the clock
            stopWatch_ = IceUtil::Time::now();
            // which object_ is it?
            int id = object->timeStamp.seconds;
            receives_[id] = stopWatch_;
            // extract the sequential ID stored in the seconds field
            ids_[id] = object->timeStamp.seconds;
            break;
        }
        } // switch
        // print out here does not affect timing
//         std::cout<<"sent id:"<<i<<std::endl;

        // NOTE: interval (period) is approximate, assume send process happens quickly
        // should be ok for longish intervals.
        // if this is a problem, estimate how much time has passed
//         std::cout<<"sleeping for "<<interval<<"us"<<std::endl;
        IceUtil::ThreadControl::sleep(IceUtil::Time::microSeconds(interval));
    }

    calcStats( objSize );

    return true;
}

void 
PingerI::calcStats( const int objSize )
{
    // RRT values
    std::vector<double> rrt;
    int lostCount = 0;
    for ( int i=0; i<config_.count; ++i ) {
        if ( ids_[i] != -1 ) {
            IceUtil::Time dt = receives_[i] - sends_[i];
            if ( dt < IceUtil::Time() ) {
                std::cout<<i<<" : negative time! "<<std::endl;
            }
            rrt.push_back( dt.toSecondsDouble() / double(config_.preload) );

            // debug
            std::cout<<i<<" snd "<<sends_[i].toSeconds()<<":"<<sends_[i].toMicroSeconds()-sends_[i].toSeconds()*1000000
                        <<" rcv "<<receives_[i].toSeconds()<<":"<<receives_[i].toMicroSeconds()-receives_[i].toSeconds()*1000000
                        <<" rtt "<<rrt.back()<<" "<<std::endl;
            // debug
            //double dd = rrt.back();
            //std::cout<<"object "<<i<<" rtt: "<<rrt.back()<<"s "<<dd<<"s "<<1000.0*dd<<"ms"<<std::endl;
        }
        else {
            ++lostCount;
            //std::cout<<"object "<<i<<" was lost"<<std::endl;
        }
    }

    // check for empty vector
    if ( rrt.empty() ) {
        std::cout<<"No statistics. All packets lost?"<<std::endl;
        return;
    }

#ifdef WIN32
    // under windows, times under 1ms cannot be measured apparently
    for ( unsigned int i=0; i<rrt.size(); ++i ) {
        if ( rrt[i] == 0 ) {
            // set to sensing limit: 1ms
            rrt[i] = 1.0e-3;
        }
    }
#endif

    // find min / max RRT's
    double rrtmin = *(std::min_element( rrt.begin()+1, rrt.end() ));
    double rrtmax = *(std::max_element( rrt.begin()+1, rrt.end() ));
    // find average RRT
    double rrtavg = 0.0;
    // ignore the first result, it's always an outlier (because the Serializer is adjusting its size)
    for ( unsigned int i=1; i<rrt.size(); ++i ) {
        rrtavg += rrt[i];
    }
    rrtavg /= (rrt.size()-1);

    // print out results (ping style)
    std::cout<<"size "<<objSize<<" bytes"<<std::endl;
    std::cout<<config_.count<<" packets transmitted, "
            <<config_.count-lostCount<<" received, "
            <<(double)lostCount/(double)config_.count*100.00<<"% packet loss, time "<<"?"<<"ms"<<std::endl;

    std::cout<<"rtt min/avg/max/mdev = "
            <<rrtmin*1000.0<<"/"<<rrtavg*1000.0<<"/"<<rrtmax*1000.0<<"/? ms"<<std::endl<<std::endl;

    // log results
    *logfile_ <<objSize<<" "
            << (rrtmin*1000.0)<<" "<<(rrtavg*1000.0)<<" "<<(rrtmax*1000.0)<<" "<<-1.0;
    // log individual rrt's
    for ( unsigned int i=1; i<rrt.size(); ++i ) {
        *logfile_ << " " << rrt[i]*1000.0;
        // debug
        //std::cout<<rrt[i]<<" ";
    }
    for ( unsigned int i=0; i<(config_.count-rrt.size()); ++i ) {
        *logfile_<< " " << -1;
    }
    *logfile_ << std::endl;

    std::cout<<std::endl;
    std::cout<<std::endl;
   
}
