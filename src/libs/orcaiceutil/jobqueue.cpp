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
        Worker( JobQueue &q, orcaiceutil::Tracer& tracer );
        virtual void run();

    private:
        JobQueue         &q_;
        orcaiceutil::Tracer&   tracer_;
    };

    Worker::Worker( JobQueue &q, orcaiceutil::Tracer& tracer )
            : q_(q),
              tracer_(tracer) 
    {
    }

    void Worker::run()
    {
        while ( isActive() )
        {
            JobPtr c = q_.getJob();
            if ( c == 0 )
            {
                IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(1));
                continue;
            }
            try {
                // cout<<"TRACE(jobqueue.cpp): worker: processing " << c->toString() << endl;
                
                JobStatusPtr s = c->process();
                q_.addJobStatus(s);
                continue;
            }
//             catch ( Ice::CommunicatorDestroyedException &e )
//             {
//                 // This is OK, we must be shutting down.
//             }
            catch ( IceUtil::Exception & e )
            {
                std::stringstream ss;
                ss << "Worker::run(): Caught exception during Job: " << c->toString();
                ss << ": " << e;
                tracer_.warning( ss.str() );
                JobStatusPtr status = new FailedJobStatus( c->jobQueueUser(), ss.str() );
                q_.addJobStatus( status );
            }
            catch ( std::exception & e )
            {
                std::stringstream ss;
                ss << "Worker::run(): Caught exception during Job: " << c->toString();
                ss << e.what();
                cout<<"TRACE(jobqueue.cpp): ss: " << ss.str() << endl;
                tracer_.warning( ss.str() );
                JobStatusPtr status = new FailedJobStatus( c->jobQueueUser(), ss.str() );
                q_.addJobStatus( status );
            }
            catch ( ... )
            {
                std::stringstream ss;
                ss << "Worker::run(): Caught unknown exception during Job: " << c->toString();
                tracer_.warning( ss.str() );
                JobStatusPtr status = new FailedJobStatus( c->jobQueueUser(), ss.str() );
                q_.addJobStatus( status );
            } // try
        } // while
    } // function
}

//////////////////////////////////////////////

JobQueue::JobQueue( orcaiceutil::Tracer& tracer )
    : tracer_(tracer)
{
}

JobQueue::~JobQueue()
{
    orcaiceutil::stopAndJoin( worker_ );
}

void
JobQueue::start()
{
    worker_ = new Worker( *this, tracer_ );
    worker_->start();
}

void
JobQueue::addJob( JobPtr &job )
{
    IceUtil::Mutex::Lock lock(mutex_);

    pendingJobs_.push_back( job );
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

    JobPtr c = pendingJobs_.front();
    pendingJobs_.pop_front();
    return c;
}

void
JobQueue::addJobStatus( JobStatusPtr &c )
{
    if ( c == 0 ) return;

    IceUtil::Mutex::Lock lock(mutex_);

    map<JobQueueUser*,JobStatusList>::iterator it = finishedJobs_.find( c->jobQueueUser() );
    if ( it == finishedJobs_.end() )
    {
        // this is a new queue user
        JobStatusList newList;
        newList.push_back( c );
        finishedJobs_[c->jobQueueUser()] = newList;
    }
    else
    {
        // this queue user already exists
        it->second.push_back( c );
    }
}

} // namespace
