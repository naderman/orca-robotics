#ifndef ORCALOG_MASTERFILEBREADCRUMBS_H
#define ORCALOG_MASTERFILEBREADCRUMBS_H

#include <fstream>
#include <vector>
#include <IceUtil/IceUtil.h>

namespace orcalog {

enum SeekResult {
    SeekOK,
    SeekQueryInFuture,
    SeekQueryBeforeStart
};

//!
//! @author Alex Brooks
//!
class MasterFileBreadCrumbs
{
public: 

    MasterFileBreadCrumbs();

    // As you move through the master file, call this to leave a trail
    // of bread-crumbs so that rewind is possible.
    // 'pos' can be read with 'tellg()' (STL book p.634)
    void placeCrumb( const std::ios::pos_type &pos, int sec, int usec );

    // Tries to find the first marker after (or at) the query time.
    // Returns:
    //   SeekOK: valid answer returned in 'pos'.
    //   SeekQueryInFuture : there was no marker at or after the query time (ie the query is in the future).
    SeekResult getCrumbAtOrAfterTime( int querySec, int queryUsec, std::ios::pos_type &pos );

    // Tries to find the first marker before the query time.
    // Returns:
    //   SeekOK: valid answer returned in 'pos'.
    //   SeekQueryBeforeStart : there was no marker before the query time (ie the query is before the start).
    SeekResult getCrumbBeforeTime( int querySec, int queryUsec, std::ios::pos_type &pos );

private: 

    bool haveCrumbAtTime( const IceUtil::Time &t );

    std::map<IceUtil::Time,std::ios::pos_type> crumbs_;

    IceUtil::Time latestTime_;
};

}

#endif
