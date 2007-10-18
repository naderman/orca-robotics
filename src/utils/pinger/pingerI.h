/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_PINGER_I_H
#define ORCA2_PINGER_I_H

#include <Ice/Ice.h>
#include <IceUtil/Time.h>

#include <vector>
#include <string>
#include <fstream>

#include <orca/pingreply.h>
#include <orcaice/context.h>

typedef struct
{
    int count;
    // sending period in [sec]
    double interval;
    int preload;
    // do we need deadline_?
} ping_config_t;

class PingerI : public orca::Pinger
{
public:
    PingerI( const orcaice::Context & context, orca::ReplierPrx &, std::ofstream* logfile, ping_config_t config );
    virtual ~PingerI() {};

    // from orca::Pinger
    virtual void callback(const orca::OrcaObjectPtr&, const Ice::Current&);

    virtual void takeItBack(const orca::PingReplyData&, const Ice::Current&);

    // local API
    // Return true if pass, false if fail
    bool sequencePing( int operationType );
    bool classPing();
//     bool structPing();
    bool icePing();

private:

    bool internalPing( orca::OrcaObjectPtr & object, const int size, int pingMode);
    void calcStats( const int size );

    orcaice::Context context_;

    // the server
    orca::ReplierPrx & replier_;

    // pointer to the log file, passed down from the calling function
    std::ofstream* logfile_;

    // ping options
    ping_config_t config_;

    std::vector<IceUtil::Time> sends_;
    std::vector<IceUtil::Time> receives_;
    std::vector<int> ids_;

    IceUtil::Time stopWatch_;

};

// define a smart pointer using Ice utilities
typedef IceUtil::Handle<PingerI> PingerIPtr;

#endif
