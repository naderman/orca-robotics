/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <algorithm>
#include <iostream>
#include <cmath>

// #include <orca/laserscanner2d.h>
// #include <orca/ogmap.h>
// #include <orca/camera.h>

#include <orcaice/orcaice.h>

#include "latencypingerI.h"

using namespace std;

LatencyPingerI::LatencyPingerI( const orcaice::Context& context, 
                    orca::util::LatencyReplierPrx & replier,
                    const Config& config ) :
    replier_(replier),
    config_(config),
    context_(context)
{
    logfile_ = new std::ofstream( config_.filename.c_str() );

    // preallocate send/receive times
    for ( int i=0; i<config_.count; ++i )
    {
        sends_.push_back( IceUtil::Time() );
        receives_.push_back( IceUtil::Time() );
        ids_.push_back( -1 );       // magic number: if these stay "-1", means object_ lost
    }
}

void 
LatencyPingerI::takeItBack(const orca::util::PingReplyData& data, const Ice::Current&)
{
    // stop the clock
    stopWatch_ = IceUtil::Time::now();

    receives_[data.id] = stopWatch_;
    ids_[data.id] = data.id;

    // printout here does not affect timing
    std::cout<<"got something back: "<<data.id<<std::endl;
}

bool 
LatencyPingerI::sequencePing( int operationType )
{
    //std::cout<<std::endl<<"object: "<<objectName_<<std::endl;

    // convert from [s] to [us]
    int interval = (int)floor( config_.interval * 1.0e6 );
    
    for ( int j=0; j<6; ++j )
    {
        // create ping object
        orca::util::PingReplyData data;
        for ( int k=0; k<pow( 10.0, double(j) ); ++k ) {
            data.doubles.push_back( k/33.0 );
        }
        // ID is stored as an int
        int objSize = sizeof(Ice::Int) + data.doubles.size()*sizeof(Ice::Double);

        // create return object
        orca::util::PingReplyData returnData;

        for ( int i=0; i<config_.count; ++i )
        {
    
            switch ( operationType )
            {
            case 1 : // ONEWAY WITH CALLBACK
            {
                // set object sequential number
                data.id = i;
                // time stamp right before sending
                sends_[i] = IceUtil::Time::now();
                replier_->takeIt( data );
                // receive times are recorded in the LatencyPingerI::callback (same host/clock)
                break;
            }
            case 2 :    // TWOWAY
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
LatencyPingerI::icePing()
{
    //std::cout<<std::endl<<"object: "<<objectName_<<std::endl;

    // convert from [s] to [us]
    int interval = (int)floor( config_.interval * 1.0e6 );
    
    for ( int i=0; i<config_.count; ++i )
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

        IceUtil::ThreadControl::sleep(IceUtil::Time::microSeconds(interval));
    }

    // is there object size for iceping?
    calcStats( 1 );
    return true;
}

void 
LatencyPingerI::calcStats( const int objSize )
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
