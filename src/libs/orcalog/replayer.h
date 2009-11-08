/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_ORCALOG_REPLAYER_H
#define ORCA_ORCALOG_REPLAYER_H

#include <string>
#include <orcalog/orcalog.h>

namespace orcalog
{

//! Handles a file which contains actual data (ie not the master file).
//! All replayers inherit from this. 
class Replayer
{
public:

    virtual ~Replayer() {}

    virtual void init() {}

    //! Replays the index'th item in the log
    virtual void replayData( int index )=0;

    //! Human-readable string describing the replayer.
    virtual std::string toString() const=0;
    
};

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////


// fwd declarations (ignore, they're defined below)
template<class LogReaderType,
         class IfaceImplType>
class DefaultIfaceImplSetup;
template<class LogReaderType,
         class IfaceImplType,
         class DescriptionType>
class DescriptionIfaceImplSetup;


//////////////////////////////////////////////////////////////////////


//
// Generic Replayer.  The last template argument is a class whose constructor
// will set up the IfaceImpl (including reading a description if necessary).
//
template< class IfaceImplType,
          class LogReaderType,
          class DataType,
          class IfaceImplSetup=DefaultIfaceImplSetup<LogReaderType,IfaceImplType> >
class GenericReplayer : public orcalog::Replayer
{
public:

    GenericReplayer( const orcalog::LogReaderInfo &logReaderInfo )
        : logReader_( logReaderInfo )
        {}
    virtual ~GenericReplayer() {}

    void init()
        {
            logReader_.init();
            IfaceImplSetup( logReader_, iface_ );
            iface_->initInterface();
        }

    void replayData( int index )
        {
            logReader_.setLogIndex( index );
            logReader_.read( data_ );
            // Don't setAndSend it: otherwise the interface will
            // remember the thing we last sent, which will behave
            // badly if we rewind the log then reconnect a client:
            // they'll get pushed something from the future.
            // iface_->localSetAndSend( data_ );
            iface_->localSend( data_ );
        }

    std::string toString() const { return logReader_.toString(); }

protected:

    IceUtil::Handle<IfaceImplType> iface_;
    LogReaderType                  logReader_;
    DataType                       data_;

};

//
// Useful defaults for setting up the IfaceImpl:
//

//
// This default will set up the IfaceImpl with no description required.
//
template<class LogReaderType,
         class IfaceImplType>
class DefaultIfaceImplSetup {
public:
    DefaultIfaceImplSetup( LogReaderType                  &logReader,
                           IceUtil::Handle<IfaceImplType> &iface )
        {
            iface = new IfaceImplType( logReader.logReaderInfo().context,
                                       logReader.logReaderInfo().interfaceName );
        }
};
//
// This version works when all that needs to happen is that a description
// needs to be read in.
//
template<class LogReaderType,
         class IfaceImplType,
         class DescriptionType>
class DescriptionIfaceImplSetup {
public:
    DescriptionIfaceImplSetup( LogReaderType                  &logReader,
                               IceUtil::Handle<IfaceImplType> &iface )
        {
            DescriptionType descr;
            logReader.read( descr );
            iface = new IfaceImplType( descr,
                                       logReader.logReaderInfo().context,
                                       logReader.logReaderInfo().interfaceName );
        }
};


} // namespace

#endif
