/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCAICEUTIL_JOB_QUEUE_H
#define ORCAICEUTIL_JOB_QUEUE_H

#include <IceUtil/Handle.h>
#include <IceUtil/Shared.h>
#include <IceUtil/Mutex.h>
#include <orcaiceutil/thread.h>
#include <orcaiceutil/tracer.h>
#include <map>
#include <list>

namespace orcaiceutil {

//
// We use the pointer to this class as a unique ID.
//
class JobQueueUser {
public:

};

//
// Describes what happened with a Job
//
class JobStatus : public IceUtil::Shared
{
public:
    JobStatus( JobQueueUser *s )
        : jobQueueUser_(s)
        {}
    virtual ~JobStatus() {}
    virtual std::string toString() const=0;
    // Did everything go smoothly?
    virtual bool isOk() const=0;

    JobQueueUser *jobQueueUser() { return jobQueueUser_; }

private:
    JobQueueUser *jobQueueUser_;
};
typedef IceUtil::Handle<JobStatus> JobStatusPtr;

//
// A request for a long-term (eg network) operation
//
class Job : public IceUtil::Shared
{
public:
    Job( JobQueueUser *s )
        : jobQueueUser_(s)
        {}
    virtual ~Job() {}

    virtual JobStatusPtr process()=0;

    virtual std::string toString() const=0;

    JobQueueUser *jobQueueUser() const { return jobQueueUser_; }

private:
    JobQueueUser *jobQueueUser_;
};
typedef IceUtil::Handle<Job> JobPtr;

//!
//! @brief A queue of calls to be processed.
//!
//! Rather than tie up the main thread, calls which may take a long time can be placed on this
//! job queue.  They will be processed by a separate thread.
//! A JobQueueUser should poll the JobQueue to see when its call is done.
//!
//! @author Alex Brooks
//!
class JobQueue :  private ::IceUtil::noncopyable
{

public: 

    //! Consturtor. Initializes the worker pool (currently fixed to size 1).
    JobQueue( orcaiceutil::Tracer& tracer );

    //! Shuts down the worker pool.
    ~JobQueue();

    //! Start the worker(s)
    void start();

    //! Add request to execute a call.
    void addJob( JobPtr &call );

    //! Returns the oldest of possibly multiple call status objects and removes
    //! it from the internal storage.
    //! Returns 0 if no call status is available for this jobQueueUser.
    JobStatusPtr getJobStatus( JobQueueUser *jobQueueUser );

    //
    // No doxy-tags because these are only used by workers.
    //

    // The worker asks for a call to work on.
    // Immediately returns 0 if no call is available.
    JobPtr getJob();

    // The worker reports status of a processed call.
    void addJobStatus( JobStatusPtr &c );

private: 
    // incoming call requests
    std::list<JobPtr>               pendingJobs_;

    typedef std::list<JobStatusPtr>       JobStatusList;
    // status of calls which were processed but the user has not asked for them yet
    std::map<JobQueueUser*,JobStatusList> finishedJobs_;

    // worker "pool" of 1.
    orcaiceutil::ThreadPtr       worker_;

    // makes internal storage thread-safe
    IceUtil::Mutex   mutex_;

    orcaiceutil::Tracer&         tracer_;
};

//
// Generic "everything worked"
//
class OkJobStatus : public JobStatus
{
public:
    OkJobStatus( JobQueueUser *s )
        : JobStatus(s) {}

    std::string toString() const { return "OkJobStatus"; }
    bool isOk() const { return true; }
};

//
// Generic "Job failed"
//
class FailedJobStatus : public JobStatus
{
public:
    FailedJobStatus( JobQueueUser *s, const std::string &what )
        : JobStatus(s),
          what_(what)
        {}

    std::string toString() const { return "FailedJobStatus: "+what(); }
    bool isOk() const { return false; }

    const std::string &what() const { return what_; }

private:
    std::string what_;
};

} // namespace

#endif
