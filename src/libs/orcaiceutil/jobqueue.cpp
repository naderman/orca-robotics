/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include "jobqueue.h"
#include <IceUtil/Time.h>
#include <iostream>

using namespace std;

namespace orcaiceutil {

//////////////////////////////////////////////

namespace {
    
    class Worker : public orcaiceutil::Thread
    {
    public:
        Worker( JobQueue &q, orcaiceutil::Tracer* tracer );
        virtual void run();

    private:
        JobQueue         &q_;
        orcaiceutil::Tracer*   tracer_;
    };

    Worker::Worker( JobQueue &q, orcaiceutil::Tracer* tracer )
            : q_(q),
              tracer_(tracer) 
    {
    }

    void Worker::run()
    {
        while ( !isStopping() )
        {
            JobPtr job = q_.getJob();
            if ( job == 0 )
            {
                IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(1));
                continue;
            }
            try {
                // cout<<"TRACE(jobqueue.cpp): worker: processing " << job->toString() << endl;
                
                JobStatusPtr s = job->execute();
                q_.addJobStatus(s);
                continue;
            }
            // would have to include Ice to be able to catch this one specifically
//             catch ( Ice::CommunicatorDestroyedException &e )
//             {
//                 // This is OK, we must be shutting down.
//             }
            catch ( IceUtil::Exception & e )
            {
                if ( tracer_ ) {
                    std::stringstream ss;
                    ss << "Worker::run(): Caught exception during Job: " << job->toString();
                    ss << ": " << e;
                    tracer_->warning( ss.str() );
                    JobStatusPtr status = new FailedJobStatus( job->jobQueueUser(), ss.str() );
                    q_.addJobStatus( status );
                }
            }
            catch ( std::exception & e )
            {
                if ( tracer_ ) {
                    std::stringstream ss;
                    ss << "Worker::run(): Caught exception during Job: " << job->toString();
                    ss << e.what();
                    cout<<"TRACE(jobqueue.cpp): ss: " << ss.str() << endl;
                    tracer_->warning( ss.str() );
                    JobStatusPtr status = new FailedJobStatus( job->jobQueueUser(), ss.str() );
                    q_.addJobStatus( status );
                }
            }
            catch ( ... )
            {
                if ( tracer_ ) {
                    std::stringstream ss;
                    ss << "Worker::run(): Caught unknown exception during Job: " << job->toString();
                    tracer_->warning( ss.str() );
                    JobStatusPtr status = new FailedJobStatus( job->jobQueueUser(), ss.str() );
                    q_.addJobStatus( status );
                }
            } // try
        } // while
    } // function
}

//////////////////////////////////////////////

JobQueue::JobQueue( orcaiceutil::Tracer* tracer, int threadPoolSize,
              int queueSizeWarn  ) : 
    tracer_(tracer),
    queueSizeWarn_(queueSizeWarn)
{
    if ( threadPoolSize<1 )
        threadPoolSize = 1;
    
    for ( size_t i=0; i<(size_t)threadPoolSize; ++i ) {
        orcaiceutil::ThreadPtr t = new Worker( *this, tracer_ );
        workerPool_.push_back( t );
    }
}

JobQueue::~JobQueue()
{    
    for ( size_t i=0; i<workerPool_.size(); ++i ) {
        orcaiceutil::stopAndJoin( workerPool_[i] );
    }
}

void
JobQueue::start()
{
    for ( size_t i=0; i<workerPool_.size(); ++i ) {
        workerPool_[i]->start();
    }
}

void
JobQueue::addJob( JobPtr &job )
{
    IceUtil::Mutex::Lock lock(mutex_);

    pendingJobs_.push_back( job );
    
    if ( tracer_ && queueSizeWarn_>0 && pendingJobs_.size()>=(size_t)queueSizeWarn_ ) {
        std::stringstream ss;
        ss << "Job queue size " << pendingJobs_.size();
        tracer_->warning( ss.str() );
    }
}

JobStatusPtr
JobQueue::getJobStatus( JobQueueUser *s )
{
    IceUtil::Mutex::Lock lock(mutex_);

    map<JobQueueUser*,JobStatusList>::iterator it = finishedJobs_.find( s );

    // unknown job queue user
    if ( it == finishedJobs_.end() )
        return 0;

    // the job queue user is known, but we have no job status objects for him
    if ( it->second.empty() )
        return 0;

    // get the oldest job status and remove it from our storage
    JobStatusPtr ret = it->second.front();
    it->second.pop_front();

    return ret;
}

JobPtr
JobQueue::getJob()
{
    IceUtil::Mutex::Lock lock(mutex_);

    if ( pendingJobs_.empty() )
        return 0;

    JobPtr job = pendingJobs_.front();
    pendingJobs_.pop_front();
    return job;
}

void
JobQueue::addJobStatus( JobStatusPtr &s )
{
    if ( s == 0 ) return;

    IceUtil::Mutex::Lock lock(mutex_);

    map<JobQueueUser*,JobStatusList>::iterator it = finishedJobs_.find( s->jobQueueUser() );
    if ( it == finishedJobs_.end() )
    {
        // this is a new queue user
        JobStatusList newList;
        newList.push_back( s );
        finishedJobs_[s->jobQueueUser()] = newList;
    }
    else
    {
        // this queue user already exists
        it->second.push_back( s );
    }
}

} // namespace
