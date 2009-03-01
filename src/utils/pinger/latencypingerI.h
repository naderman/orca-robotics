/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_LATENCY_PINGER_I_H
#define ORCA_LATENCY_PINGER_I_H

#include <IceUtil/Time.h>

#include <vector>
#include <string>
#include <fstream>

#include "pingreply.h"
#include <orcaice/context.h>

class LatencyPingerI : public orca::util::LatencyPinger
{
public:
    struct Config
    {
        int count;
        // sending period in [sec]
        double interval;
        int preload;
        // do we need deadline_?
        std::string filename;
    };

    LatencyPingerI( const orcaice::Context& context, orca::util::LatencyReplierPrx& replier, const Config& config );

    // from orca::util::LatencyPinger
    virtual void takeItBack(const orca::util::PingReplyData&, const Ice::Current&);

    // local API
    // Return true if pass, false if fail
    bool sequencePing( int operationType );
//     bool classPing();
//     bool structPing();
    bool icePing();

private:
    void calcStats( const int size );

    // the server
    orca::util::LatencyReplierPrx& replier_;

    // pointer to the log file, passed down from the calling function
    std::ofstream* logfile_;

    // ping options
    Config config_;

    std::vector<IceUtil::Time> sends_;
    std::vector<IceUtil::Time> receives_;
    std::vector<int> ids_;

    IceUtil::Time stopWatch_;

    orcaice::Context context_;
};

// define a smart pointer using Ice utilities
typedef IceUtil::Handle<LatencyPingerI> LatencyPingerIPtr;

#endif
