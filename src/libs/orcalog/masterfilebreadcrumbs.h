#ifndef ORCALOG_MASTERFILEBREADCRUMBS_H
#define ORCALOG_MASTERFILEBREADCRUMBS_H

#include <fstream>
#include <vector>
#include <IceUtil/IceUtil.h>

namespace orcalog {

// //
// // This structure stores the position in the file
// // of the beginning of the record with the given timestamp.
// //
// class MasterFileBreadCrumb {
// public:
//     MasterFileBreadCrumb() {}
//     MasterFileBreadCrumb( const std::ios::pos_type &thePos,
//                           const IceUtil::Time      &theTime )
//         : pos(thePos),
//           time(theTime)
//         {}

//     std::ios::pos_type pos;
//     IceUtil::Time time;
// };

enum SeekResult {
    SeekOK,
    SeekQueryInFuture
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
    //   SeekQueryInFuture : there was no marker after the query time (ie the query is in the future).
    SeekResult getCrumbAtOrAfterTime( int querySec, int queryUsec, std::ios::pos_type &pos );

private: 

    bool haveCrumbAtTime( const IceUtil::Time &t );

    std::map<IceUtil::Time,std::ios::pos_type> crumbs_;

    IceUtil::Time latestTime_;
};

}

#endif
