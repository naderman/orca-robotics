#ifndef ORCALOG_SNAPSHOTLOGBUFFER_H
#define ORCALOG_SNAPSHOTLOGBUFFER_H

#include <orca/orca.h>
#include <deque>
#include <orcalog/logwriter.h>
#include <orcaice/timeutils.h>

namespace orcalog {

//!
//! @brief A fixed-length (in time) circular buffer.
//!
//! @author Alex Brooks
//!
template<typename ObjectType,typename LogWriterType>
class SnapshotLogBuffer
{

public: 

    SnapshotLogBuffer( double timeWindowSec )
        : timeWindow_(timeWindowSec)
        {}

    //! Adds the item to the buffer, ensuring that the timeWindow is respected
    void addItem( const ObjectType &obj )
        {
            buffer_.push_back( LogItem( obj, orcaice::getNow() ) );

            // std::cout<<"TRACE(snapshotlogbuffer.h): new time window: " << 
            // orcaice::timeDiffAsDouble( buffer_.back().arrivalTime, buffer_.front().arrivalTime ) << std::endl;

            // Delete old stuff if there's too much in the buffer
            while ( orcaice::timeDiffAsDouble( buffer_.back().arrivalTime, buffer_.front().arrivalTime ) > timeWindow_ )
            {
                buffer_.pop_front();
                // std::cout<<"TRACE(snapshotlogbuffer.h): purgind old data." << std::endl;
            }
        }

    const uint bufferSize() const
        { return buffer_.size(); }

    const orca::Time &oldestArrivalTime() const
        { 
            assert(buffer_.size()>0);
            return buffer_.front().arrivalTime;
        }
    void writeOldestObjToLogAndDiscard( LogWriterType &logWriter )
        {
            assert( buffer_.size()>0 );
            logWriter.write( buffer_.front().obj, buffer_.front().arrivalTime );
            buffer_.pop_front();
        }
            
private: 

    struct LogItem {
        LogItem() {}
        LogItem( const ObjectType &o, const orca::Time &t )
            : obj(o), arrivalTime(t)
            {}

        ObjectType obj;
        orca::Time arrivalTime;
    };

    std::deque<LogItem> buffer_;
    double timeWindow_;

};

}

#endif
