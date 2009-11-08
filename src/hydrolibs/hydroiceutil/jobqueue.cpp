/*
 * Orca-Robotics Project: Components for robotics
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include "jobqueue.h"
#include <IceUtil/Time.h>
#include <gbxsickacfr/gbxiceutilacfr/safethread.h>
#include <gbxsickacfr/gbxiceutilacfr/timer.h>
#include <gbxutilacfr/exceptions.h>
#include <iostream>

using namespace std;

namespace hydroiceutil {

//////////////////////////////////////////////

namespace {

    class Worker : public gbxiceutilacfr::SafeThread
    {
    public:
        Worker( JobQueue &q, gbxutilacfr::Tracer& tracer, bool traceStartEvents, bool traceDoneEvents );

        // all of the following are thread safe (acquire jobMutex_)

        void takeJob( const JobPtr& newJob );
        void dropJob();
        // does nothing if the worker has no current job
        void cancelJob();
        // returns FALSE if the worker has no current job
        bool isJobStalled( std::string& jobInfo );
        // returns empty string if the worker has no current job
        std::string toString();

    private:

        virtual void walk();

        JobQueue& q_;
        JobPtr job_;
        IceUtil::Mutex jobMutex_;
        gbxiceutilacfr::Timer jobTimer_;

        gbxutilacfr::Tracer& tracer_;
        bool traceStartEvents_;
        bool traceDoneEvents_;
    };
    typedef IceUtil::Handle<Worker> WorkerPtr;

    Worker::Worker( JobQueue &q, gbxutilacfr::Tracer& tracer, bool traceStartEvents, bool traceDoneEvents ) :
        SafeThread( tracer ),
        q_(q),
        tracer_(tracer),
        traceStartEvents_(traceStartEvents),
        traceDoneEvents_(traceDoneEvents)
    {
    }

    void Worker::walk()
    {
        // stream for collecting exception errors
        stringstream exceptionSS;

        // this is the main loop
        while ( !isStopping() )
        {
            // newJob will drop out of scope after {}
            {
                // blocks until a new job arrives
                JobPtr newJob = q_.get( this );

                // we woke up but there's no job
                // this probably means that we are stopping, but may as well go to the top of while
                if ( newJob == 0 )
                    continue;

                // make this job officially ours
                takeJob( newJob );
            }

            try
            {
                if ( job_->isTracable_ || traceStartEvents_ ) {
                    stringstream ss;
                    ss << "Worker "<<this->getThreadControl().id()<<": starting job "<<job_->toString();
                    tracer_.info( ss.str() );
                }

                //
                // do our job
                //
                job_->execute();

                if ( job_->isTracable_ || traceDoneEvents_ ) {
                    stringstream ss;
                    ss << "Worker "<<this->getThreadControl().id()<<": finished executing job "<<job_->toString();
                    tracer_.info( ss.str() );
                }

                // forget about this job
                // this is safer in case it holds pointers to resources which may become unavailable
                dropJob();

                // all good, go to the top while and get another job
                continue;
            }
            // would have to include Ice to be able to catch this one specifically
//             catch ( Ice::CommunicatorDestroyedException &e )
//             {
//                 // This is OK, we must be shutting down.
//             }
            catch ( IceUtil::Exception& e ) {
                exceptionSS << "Worker::walk(): Caught exception during Job: " << job_->toString();
                exceptionSS << ": " << e;
            }
            catch ( std::exception& e ) {
                exceptionSS << "Worker::walk(): Caught exception during Job: " << job_->toString();
                exceptionSS << ": " << e.what();
            }
            catch ( ... ) {
                exceptionSS << "Worker::walk(): Caught unknown exception during Job: " << job_->toString();
            } // try

            // we only get here after an exception
            tracer_.warning( exceptionSS.str() );
            exceptionSS.str("");
            dropJob();
        } // while

    } // walk

    void Worker::takeJob( const JobPtr& newJob )
    {
        IceUtil::Mutex::Lock lock(jobMutex_);
        job_ = newJob;
        jobTimer_.restart();
    }

    void Worker::dropJob()
    {
        IceUtil::Mutex::Lock lock(jobMutex_);
        job_ = 0;
    }

    void Worker::cancelJob()
    {
        IceUtil::Mutex::Lock lock(jobMutex_);
        if ( job_ )
            job_->cancel();
    }

    bool Worker::isJobStalled( std::string& jobInfo )
    {
        IceUtil::Mutex::Lock lock(jobMutex_);
        if ( job_ &&
             job_->expectedDuration() >= 0 &&
             (double)job_->expectedDuration() < jobTimer_.elapsedMs() )
        {
            std::stringstream ss;
            ss << "Job " << job_->toString() << "has been running for "
               << jobTimer_.elapsedMs() << "ms (expected " << job_->expectedDuration() << "ms)";
            jobInfo = ss.str();
            return true;
        }

        return false;
    }

    std::string Worker::toString()
    {
        string s;
        IceUtil::Mutex::Lock lock(jobMutex_);
        if ( job_ )
            s = job_->toString();

        return s;
    }
}

//////////////////////////////////////////////

Job::Job() :
    isTracable_(false),
    isCancelled_(false)
{
}

void
Job::cancel()
{
    IceUtil::Mutex::Lock lock(isCancelledMutex_);
    isCancelled_ = true;
}

bool
Job::isCancelled()
{
    IceUtil::Mutex::Lock lock(isCancelledMutex_);
    return isCancelled_;
}

//////////////////////////////////////////////

void
JobQueue::Config::validate( gbxutilacfr::Tracer& tracer )
{
    if ( threadPoolSize<1 ) {
        stringstream ss;
        ss << "Configured to have thread pool size of "<<threadPoolSize<<". Must be a positive number";
        throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
    }
}

//////////////////////////////////////////////

JobQueue::JobQueue( gbxutilacfr::Tracer& tracer, const Config& config ) :
    isQueueStopping_(false),
    tracer_(tracer),
    config_(config)
{
    config_.validate( tracer_ );

    // this number will go down to zero as soon all workers are initialized
    activeJobCount_ = config_.threadPoolSize;

    for ( size_t i=0; i<(size_t)config_.threadPoolSize; ++i ) {
        gbxiceutilacfr::ThreadPtr t = new Worker( *this, tracer_, config_.traceStartEvents, config_.traceDoneEvents );
        t->start();
        workerPool_.push_back( t );
    }
}

JobQueue::~JobQueue()
{
    // if the user called stop() than all worder threads would already be dead
    // otherwise, some will still be alive, call stop()

    bool liveWorkers = false;
    for ( size_t i=0; i<workerPool_.size(); ++i )  {
        if ( workerPool_[i] ) {
            liveWorkers = true;
            break;
        }
    }

    if ( liveWorkers )
        stop();
}

void
JobQueue::stop()
{
    // stop accepting new jobs and clear all pending jobs
    {
        IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);
        isQueueStopping_ = true;
        pendingJobs_.clear();
    }

    std::vector<IceUtil::ThreadControl> controls;

    // first tell the workers to stop and cancel their jobs (if any)
    for ( size_t i=0; i<workerPool_.size(); ++i )
    {
        if ( workerPool_[i] )
        {
            // get the control object first
            controls.push_back( workerPool_[i]->getThreadControl() );

            // Stop the worker first so it doesn't grab another job, when the current one is cancelled
            workerPool_[i]->stop();

            // if it's working this will interrupt the job
            WorkerPtr w = WorkerPtr::dynamicCast( workerPool_[i] );
            w->cancelJob();
        }
    }

    // now the workers whose jobs were cancelled have already exited
    // those who are waiting needs to be woken up
    {
        IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);
        notifyAll();
    }

    // now all workers should be shutting down
    // make sure that we wait until they all exit

    for ( size_t i=0; i<controls.size(); ++i )
    {
//         cout<<"JobQueue::stop(): waiting for thread "<<i<<endl;
        controls[i].join();
    }

    // now we can forget about the workers
    // this is important because stop() may be called twice: by the end-user and the destructor
    for ( size_t i=0; i<workerPool_.size(); ++i )
    {
        workerPool_[i] = 0;
    }
}

void
JobQueue::add( const JobPtr& job )
{
    IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);

    if ( isQueueStopping_ )
        return;

    pendingJobs_.push_back( job );

    if ( job->isTracable_ || config_.traceAddEvents )
        tracer_.info( "Added "+job->toString()+" to the job queue" );

    if ( config_.queueSizeWarn>0 && pendingJobs_.size()>=(size_t)config_.queueSizeWarn ) {
        std::stringstream ss;
        ss << "Job queue size " << pendingJobs_.size() << " after adding "<<job->toString();
        tracer_.warning( ss.str() );
    }

    if ( config_.traceStalledJobs ) {
        std::string jobInfo;
        for ( size_t i=0; i<workerPool_.size(); ++i )
        {
            WorkerPtr w = WorkerPtr::dynamicCast( workerPool_[i] );
            if ( w->isJobStalled( jobInfo ) )
                tracer_.warning( jobInfo );
        }
    }

    notify();
}

// this function is executed in the threads of the workers
JobPtr
JobQueue::get( gbxiceutilacfr::Thread* thread )
{
    IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);

    --activeJobCount_;

    while( !thread->isStopping() && pendingJobs_.empty() )
    {
        wait();
    }

    if ( thread->isStopping() )
        return 0;

    JobPtr job = pendingJobs_.front();
    pendingJobs_.pop_front();

    ++activeJobCount_;

    return job;
}

JobQueueStatus
JobQueue::status()
{
    IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);

    JobQueueStatus status;
    status.activeJobCount = activeJobCount_;
    status.queuedJobCount = pendingJobs_.size();

    return status;
}

std::string
JobQueue::toString()
{
    stringstream ss;

    IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);

    if ( activeJobCount_ )
        ss<<"ACTIVE JOBS (maybe out of sync):"<<endl;
    for ( size_t i=0; i<workerPool_.size(); ++i )
    {
        string s;
        if ( workerPool_[i] )
        {
            WorkerPtr w = WorkerPtr::dynamicCast( workerPool_[i] );
            s = w->toString();
            if ( !s.empty() )
                ss << "  " << w->toString() << endl;
        }
    }

    if ( !pendingJobs_.empty() )
        ss<<"PENDING JOBS:"<<endl;
    for ( std::list<JobPtr>::const_iterator it=pendingJobs_.begin(); it!=pendingJobs_.end(); ++it ) {
        const JobPtr job = *it;
        ss << "  " << job->toString() << endl;
    }

    return ss.str();
}

} // namespace
