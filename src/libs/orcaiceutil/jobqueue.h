/*
 * Orca Project: Components for robotics 
 *               http://!orca-robotics.sf.net/
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

//!
//! We use the pointer to this class as a unique ID.
//!
class JobQueueUser {
public:

};

//!
//! Describes what happened with a Job
//!
class JobStatus : public IceUtil::Shared
{
public:
    //! Consturtor
    JobStatus( JobQueueUser *s )
        : jobQueueUser_(s)
        {}
    virtual ~JobStatus() {}

    //! Returns a text description of the job status
    virtual std::string toString() const=0;

    //! Did everything go smoothly?
    virtual bool isOk() const=0;

    //! Returns the pointer to the user who posted the job.
    JobQueueUser *jobQueueUser() { return jobQueueUser_; }

private:
    JobQueueUser *jobQueueUser_;
};
//! Ice smart pointer to job status.
typedef IceUtil::Handle<JobStatus> JobStatusPtr;


//!
//! A request to perform an operation which may take a long time 
//! (eg network job).
//!
class Job : public IceUtil::Shared
{
public:
    //! Consturtor
    Job( JobQueueUser *s )
        : jobQueueUser_(s)
        {}
    virtual ~Job() {}

    //! Implement this function and do your job in it.
    virtual JobStatusPtr process()=0;

    //! Returns a text description of the job
    virtual std::string toString() const=0;

    //! Returns the pointer to the user who posted the job.
    JobQueueUser *jobQueueUser() const { return jobQueueUser_; }

private:
    JobQueueUser *jobQueueUser_;
};
//! Ice smart pointer to job.
typedef IceUtil::Handle<Job> JobPtr;


//!
//! @brief A queue of jobs to be processed.
//!
//! Rather than tie up the main thread, calls which may take a long time 
//! can be placed on this job queue.  
//! They will be processed by a separate thread.
//! A JobQueueUser should poll the JobQueue to see when its job is done.
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

    //! Add request to execute a job.
    void addJob( JobPtr &job );

    //! Returns the oldest of possibly multiple job status objects and removes
    //! it from the internal storage.
    //! Returns 0 if no job status is available for this jobQueueUser.
    JobStatusPtr getJobStatus( JobQueueUser *jobQueueUser );

    //
    // No doxy-tags because these are only used by workers.
    //

    // The worker asks for a job to work on.
    // Immediately returns 0 if no job is available.
    JobPtr getJob();

    // The worker reports status of a processed job.
    void addJobStatus( JobStatusPtr &c );

private: 
    // incoming job requests
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

//!
//! Generic "everything worked" status
//!
class OkJobStatus : public JobStatus
{
public:
    //! Consturtor
    OkJobStatus( JobQueueUser *s )
        : JobStatus(s) {}

    //! It's all good and nothing else to say.
    virtual std::string toString() const { return "OkJobStatus"; }

    //! Returns the good news.
    virtual bool isOk() const { return true; }
};

//!
//! Generic "Job failed" status
//!
class FailedJobStatus : public JobStatus
{
public:
    //! Consturtor, with a reason for failure.
    FailedJobStatus( JobQueueUser *s, const std::string &what )
        : JobStatus(s),
          what_(what)
        {}

    //! Returns the reason for failure.
    virtual std::string toString() const { return "FailedJobStatus: "+what(); }

    //! Returns the bad news.
    virtual bool isOk() const { return false; }

    const std::string &what() const { return what_; }

private:
    std::string what_;
};

} // namespace

#endif
