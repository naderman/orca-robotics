/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <IceUtil/Time.h>
#include <hydroiceutil/jobqueue.h>
#include <hydroiceutil/simpletracer.h>
#include <gbxutilacfr/exceptions.h>

using namespace std;

class TestJob : public hydroiceutil::Job 
{
public:    
    virtual void execute()
    {
        // does nothing
    };

    virtual std::string toString() const
    {
        return "TestJob";
    };
};

class InfiniteJob : public hydroiceutil::Job 
{
public:    
    virtual void execute()
    {
        while( !isCancelled() )
            IceUtil::ThreadControl::sleep(IceUtil::Time::milliSeconds(200));
    };

    virtual std::string toString() const
    {
        return "InfiniteJob";
    };
};

int main(int argc, char * argv[])
{
    cout<<"starting job queue with illegal parameters ... ";
    {
        hydroiceutil::SimpleTracer tracer;
        hydroiceutil::JobQueue::Config config;
        config.threadPoolSize = -1;
        config.queueSizeWarn = 1;
        config.traceAddEvents = true;
        config.traceStartEvents = true;
        config.traceDoneEvents = true;

        try {
            hydroiceutil::JobQueue jobQueue( tracer, config );
            cout<<" failed. Should've thrown an exception."<<endl;
            return EXIT_FAILURE;
        }
        catch ( const gbxutilacfr::Exception& e ) {
        }
    }
    cout<<"ok"<<endl;

    cout<<"starting job queue with thread pool 1 and a trivial job ... ";
    {
        hydroiceutil::SimpleTracer tracer;
        hydroiceutil::JobQueue::Config config;
        config.threadPoolSize = 1;
        config.queueSizeWarn = 1;
        config.traceAddEvents = true;
        config.traceStartEvents = true;
        config.traceDoneEvents = true;
        hydroiceutil::JobQueue jobQueue( tracer, config );

        // wait for the worker to start up, this needed for the active job count to go to zero
        IceUtil::ThreadControl::sleep(IceUtil::Time::milliSeconds(100));
        hydroiceutil::JobQueueStatus status = jobQueue.status();
        if ( status.activeJobCount!=0 || status.queuedJobCount!=0 ) {
            cout<<" failed. Wrong status after activation: active="<<status.activeJobCount<<" queued="<<status.queuedJobCount<<endl;
            return EXIT_FAILURE;
        }

        hydroiceutil::JobPtr j = new TestJob;
        jobQueue.add( j );

        // wait for the job to get executed
        IceUtil::ThreadControl::sleep(IceUtil::Time::milliSeconds(100));
        jobQueue.stop();
    }
    cout<<"ok"<<endl;

    cout<<"starting job queue with 1 worker and 1 endless job ... ";
    {
        hydroiceutil::SimpleTracer tracer;
        hydroiceutil::JobQueue::Config config;
        config.threadPoolSize = 1;
        config.queueSizeWarn = 1;
        config.traceAddEvents = true;
        config.traceStartEvents = true;
        config.traceDoneEvents = true;
        hydroiceutil::JobQueue jobQueue( tracer, config );

        hydroiceutil::JobPtr j = new InfiniteJob;
        jobQueue.add( j );

        // sleep for a bit, so we can see the debug statement on when the finite job finishes
        IceUtil::ThreadControl::sleep(IceUtil::Time::milliSeconds(100));

        hydroiceutil::JobQueueStatus status = jobQueue.status();
        if ( status.activeJobCount!=1 || status.queuedJobCount!=0 ) {
            cout<<" failed. Wrong status: active="<<status.activeJobCount<<" queued="<<status.queuedJobCount<<endl;
            return EXIT_FAILURE;
        }

        jobQueue.stop();
    }
    cout<<"ok"<<endl;

    cout<<"starting job queue with 1 worker and 2 endless jobs ... ";
    {
        hydroiceutil::SimpleTracer tracer;
        hydroiceutil::JobQueue::Config config;
        config.threadPoolSize = 1;
        config.queueSizeWarn = 1;
        config.traceAddEvents = true;
        config.traceStartEvents = true;
        config.traceDoneEvents = true;
        hydroiceutil::JobQueue jobQueue( tracer, config );

        hydroiceutil::JobPtr j0 = new InfiniteJob;
        jobQueue.add( j0 );
        hydroiceutil::JobPtr j1 = new InfiniteJob;
        jobQueue.add( j1 );

        // sleep for a bit, so the first job starts executing
        IceUtil::ThreadControl::sleep(IceUtil::Time::milliSeconds(100));

        hydroiceutil::JobQueueStatus status = jobQueue.status();
        if ( status.activeJobCount!=1 || status.queuedJobCount!=1 ) {
            cout<<" failed. Wrong status: active="<<status.activeJobCount<<" queued="<<status.queuedJobCount<<endl;
            return EXIT_FAILURE;
        }

        jobQueue.stop();
    }
    cout<<"ok"<<endl;
    cout<<"starting job queue with 2 workers and 3 endless jobs and 1 trival job... ";
    {
        hydroiceutil::SimpleTracer tracer;
        hydroiceutil::JobQueue::Config config;
        config.threadPoolSize = 2;
        config.queueSizeWarn = 1;
        config.traceAddEvents = true;
        config.traceStartEvents = true;
        config.traceDoneEvents = true;
        hydroiceutil::JobQueue jobQueue( tracer, config );

        hydroiceutil::JobPtr j0 = new InfiniteJob;
        jobQueue.add( j0 );
        hydroiceutil::JobPtr j1 = new TestJob;
        jobQueue.add( j1 );
        hydroiceutil::JobPtr j2 = new InfiniteJob;
        jobQueue.add( j2 );
        hydroiceutil::JobPtr j3 = new InfiniteJob;
        jobQueue.add( j3 );

        // sleep for a bit, so we can see the debug statement on when the finite job finishes
        IceUtil::ThreadControl::sleep(IceUtil::Time::milliSeconds(100));

        hydroiceutil::JobQueueStatus status = jobQueue.status();
        if ( status.activeJobCount!=2 || status.queuedJobCount!=1 ) {
            cout<<" failed. Wrong status: active="<<status.activeJobCount<<" queued="<<status.queuedJobCount<<endl;
            return EXIT_FAILURE;
        }
        jobQueue.stop();
    }
    cout<<"ok"<<endl;
    
    return EXIT_SUCCESS;
}
