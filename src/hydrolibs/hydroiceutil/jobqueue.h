/*
 * Orca-Robotics Project: Components for robotics
 *               http://!orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef HYDROICEUTIL_JOB_QUEUE_H
#define HYDROICEUTIL_JOB_QUEUE_H

#include <IceUtil/Handle.h>
#include <IceUtil/Shared.h>
#include <IceUtil/Monitor.h>
#include <IceUtil/Mutex.h>
#include <gbxutilacfr/stoppable.h>
#include <gbxsickacfr/gbxiceutilacfr/thread.h>
#include <hydroutil/uncopyable.h>
#include <gbxutilacfr/tracer.h>
#include <vector>
#include <list>

namespace hydroiceutil {


//!
//! A request to perform an operation which may take a long time
//! (eg network job).
//!
//! Job implementation must interrupt execution when the the job is cancelled.
//! Check isCancelled() periodically.
//!
class Job : public gbxutilacfr::Stoppable, public IceUtil::Shared
{
public:
    Job();
    virtual ~Job() {}

    //! Implement this function and do your job in it.
    //! It will be done by one of the 'workers' from the queue's thread pool.
    virtual void execute()=0;

    //! Returns a text description of the job
    virtual std::string toString() const=0;

    //! Returns expected duration of the job in [ms].
    //! Returns a negative number if the job duration is infinite or unknown (default).
    //! This information can be used by the JobQueue for detection of stalled jobs.
    virtual int expectedDuration() const { return -1; };

    //! Cancel this job. Execution should stop. What to do in this case is job-dependent.
    //! Depending on the state in which the job is finished, it may be necessary to inform
    //! somebody about what did and did not happen.
    void cancel();

    //! Returns TRUE if the job is in Cancelled state, FALSE otherwise.
    bool isCancelled();

    // from gbxutilacfr::Stoppable
    //! Convenience function for compatibility with Stoppable interface.
    //! Returns the same result as isCancelled().
    virtual bool isStopping() { return isCancelled(); };

    //! If TRUE, start and end of job execution are traced to info().
    //! Default is FALSE.
    //! This is a debugging tool, use with care! Not thread safe.
    //! Normal usage: create a job, set isTracable_ to TRUE before adding
    //! the job to the job queue and do not modify it again.
    bool isTracable_;

private:

    bool isCancelled_;
    IceUtil::Mutex isCancelledMutex_;

};
//! Ice smart pointer to job.
typedef IceUtil::Handle<Job> JobPtr;

//! Current status of the job queue.
struct JobQueueStatus
{
    //! Number of jobs currently executed.
    int activeJobCount;
    //! Number of jobs waiting in the queue.
    int queuedJobCount;
};

/*!
@brief A queue of jobs to be processed.

Rather than tie up the main thread, calls which may take a long time
can be placed on this job queue. They will be processed by a pool of threads.

If you need the jobs to communicate results or status updates to their owners,
you may want to use the event mechanism, @see Event, EventReceiver.

@author Alex Brooks, Alex Makarenko
*/
class JobQueue :  public IceUtil::Shared,
                  public IceUtil::Monitor<IceUtil::Mutex>,
                  private hydroutil::Uncopyable
{
public:
    //! Job queue configuration structure
    struct Config
    {
        Config() :
            threadPoolSize(1),
            queueSizeWarn(0),
            traceAddEvents(false),
            traceStartEvents(false),
            traceDoneEvents(false),
            traceStalledJobs(false) {};

        //! Throws Exception when mis-configured.
        void validate( gbxutilacfr::Tracer& tracer );

        //! Thread pool size. Zero and negative values are not allowed, a
        //! gbxutilacfr::Exception is thrown. Default is 1.
        int threadPoolSize;
        //! When the queue of pending jobs reaches this number, a warning is issued through
        //! the Tracer. Information about the last added job is also traced.
        //! Zero and negative values produce no warnings. Default is 0;
        int queueSizeWarn;
        //! Use for debugging. If TRUE, addition of each job is traced to info().
        //! Default is FALSE.
        bool traceAddEvents;
        //! Use for debugging. If TRUE, start of execution of each job is traced to info().
        //! Default is FALSE.
        bool traceStartEvents;
        //! Use for debugging. If TRUE, completion of execution of each job is traced to info().
        //! Default is FALSE.
        bool traceDoneEvents;
        //! If TRUE, start of execution of each job is traced to info().
        //! Default is FALSE.
        bool traceStalledJobs;
//         bool cancelStalledJobs;
    };

    //! Constructor.
    //! For queueSizeWarn>0, a warning will be traced when the queue size
    //! exceeds specified size.
    //!
    //! Worker threads are started in the constructor.
    JobQueue( gbxutilacfr::Tracer& tracer, const Config& config=Config() );

    //! Calls stop() if any the worker threads are still running.
    ~JobQueue();

    //! Stops accepting new jobs, clears all pending jobs, cancels all running jobs and stops the worker pool.
    //! Will not return until all workers have stopped.
    //! Use this function instead of relying on the destructor to call it if you need to make sure that
    //! the thread pool is stopped before other objects (e.g. threads) are destroyed.
    void stop();

    //! Add request to execute a job. If add() is called after stop(), the request to add a job is quietly ignored.
    void add( const JobPtr& job );

    //! Returns current queue status.
    JobQueueStatus status();

    //! Returns a return-separated listing of all active and pending jobs.
    std::string toString();

    //
    // No doxy-tags because these are only used by workers.
    //

    // The worker asks for a job to work on.
    // Blocks until a job arrives
    JobPtr get( gbxiceutilacfr::Thread* thread );

private:
    // incoming job requests
    std::list<JobPtr>               pendingJobs_;

    // worker pool
    std::vector<gbxiceutilacfr::ThreadPtr> workerPool_;
    int activeJobCount_;

    bool isQueueStopping_;

    gbxutilacfr::Tracer& tracer_;
    Config config_;
};
//! Ice smart pointer to job queue.
typedef IceUtil::Handle<JobQueue> JobQueuePtr;

} // namespace

#endif
