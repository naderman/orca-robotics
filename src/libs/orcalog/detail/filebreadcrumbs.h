#ifndef ORCALOG_FILEBREADCRUMBS_H
#define ORCALOG_FILEBREADCRUMBS_H

#include <sstream>
#include <fstream>
#include <vector>
#include <IceUtil/IceUtil.h>
#include <orcalog/utils.h>

namespace orcalog {
namespace detail {

////////////////////////////////////////
enum SeekResult {
    SeekOK,
    SeekQueryInFuture,
    SeekQueryBeforeStart
};
////////////////////////////////////////

//
// @brief Class to leave a trail of bread-crumbs when moving sequentially through a 
//        file, so we can rewind.
//        Assumes that we start from the start and place a crumb at every point to which
//        we might later want to rewind.
//
// @author Alex Brooks
//
template<typename IndexType>
class FileBreadCrumbs
{
public: 

    // As you move through the file, call this to leave a trail
    // of bread-crumbs so that rewind is possible.
    // 'pos' can be read with 'tellg()' (STL book p.634)
    void placeCrumb( const std::ios::pos_type &pos, const IndexType &t );

    // Tries to find the first marker after (or at) the query.
    // Returns:
    //   SeekOK: valid answer returned in 'pos'.
    //   SeekQueryInFuture : there was no marker at or after the query (ie the query is in the future).
    SeekResult getCrumbAtOrAfter( const IndexType &t, std::ios::pos_type &pos );

    // Tries to find the marker exactly at the query.
    // Returns:
    //   true:  found
    //   false: not found
    bool getCrumbAt( const IndexType &t, std::ios::pos_type &pos );

    // Tries to find the last marker before the query.
    // Returns:
    //   SeekOK: valid answer returned in 'pos'.
    //   SeekQueryBeforeStart : there was no marker before the query (ie the query is before the start).
    SeekResult getCrumbBefore( const IndexType &t, std::ios::pos_type &pos );

    const IndexType &latest() const { return maxIndex_; }

private: 

    bool haveCrumbAt( const IndexType &t );

    std::map<IndexType,std::ios::pos_type> crumbs_;
    typedef typename std::map<IndexType,std::ios::pos_type>::const_iterator ConstCrumbsIterator;

    IndexType maxIndex_;
};

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

template<typename IndexType>
bool
FileBreadCrumbs<IndexType>::haveCrumbAt( const IndexType &index )
{
    ConstCrumbsIterator it =
        crumbs_.find( index );
    return !(it == crumbs_.end());
}

template<typename IndexType>
void
FileBreadCrumbs<IndexType>::placeCrumb( const std::ios::pos_type &pos,
                                              const IndexType &index )
{
    if ( crumbs_.size() == 0 )
    {
        // First crumb -- add it and make this the maxIndex_
        maxIndex_ = index;
        crumbs_[index] = pos;
    }
    else
    {
        // Already have at least one crumb.
        // Add this one if it's new.
        if ( index > maxIndex_    ||
             !haveCrumbAt(index) )
        {
            crumbs_[index] = pos;
            if ( index > maxIndex_ )
                maxIndex_ = index;
        }
    }
}

template<typename IndexType>
SeekResult
FileBreadCrumbs<IndexType>::getCrumbAtOrAfter( const IndexType &index,
                                                     std::ios::pos_type &pos )
{
    ConstCrumbsIterator it = crumbs_.lower_bound( index );
    if ( it == crumbs_.end() )
    {
        return SeekQueryInFuture;
    }
    else
    {
        pos = it->second;
        return SeekOK;
    }
}

template<typename IndexType>
bool
FileBreadCrumbs<IndexType>::getCrumbAt( const IndexType &index,
                                        std::ios::pos_type &pos )
{
    ConstCrumbsIterator it = crumbs_.find( index );
    if ( it == crumbs_.end() )
    {
        return false;
    }
    else
    {
        pos = it->second;
        return true;
    }
}

template<typename IndexType>
SeekResult 
FileBreadCrumbs<IndexType>::getCrumbBefore( const IndexType &index,
                                            std::ios::pos_type &pos )
{
    // Find the first crumb after the specified index
    ConstCrumbsIterator it = crumbs_.lower_bound( index );

    if ( it == crumbs_.begin() )
    {
        return SeekQueryBeforeStart;
    }
    else
    {
        it--;
        pos = it->second;
        return SeekOK;
    }
}

//////////////////////////////////////////////////////////////////////

inline std::string toString( SeekResult &r )
{
    std::stringstream ss;
    switch (r)
    {
    case SeekOK:
        ss << "SeekOK";
        break;
    case SeekQueryInFuture:
        ss << "SeekQueryInFuture";
        break;
    case SeekQueryBeforeStart:
        ss << "SeekQueryBeforeStart";
        break;
    default:
        ss << "Unknown SeekResult";
        break;
    }
    return ss.str();
}

} // end namespace orcalog
} // end namespace detail

#endif
