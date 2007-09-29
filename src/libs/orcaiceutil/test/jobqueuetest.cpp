/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <IceUtil/Time.h>
#include <orcaiceutil/jobqueue.h>
#include <orcaiceutil/simpletracer.h>

using namespace std;

class TestJob : public orcaiceutil::Job 
{
public:
    virtual orcaiceutil::JobStatusPtr process()
    {
        return new orcaiceutil::OkJobStatus(jobQueueUser());
    };

    virtual std::string toString() const
    {
        return "TestJob";
    };
};

int main(int argc, char * argv[])
{
    cout<<"starting job queue... ";
    orcaiceutil::SimpleTracer tracer;
    orcaiceutil::JobQueue jobQueue( tracer );
    jobQueue.start();
    cout<<"ok"<<endl;
    
    return EXIT_SUCCESS;
}
