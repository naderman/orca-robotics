/*c
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAQCM_JOBS_H
#define ORCAQCM_JOBS_H

#include <hydroutil/jobqueue.h>
#include <orcaice/context.h>
#include <QCoreApplication>

namespace orcaqcm
{

//! Contacts the registry, gets home data and posts a HomeEvent (a QEvent) to be processed by @c obj.
class GetComponentsJob : public hydroutil::Job
{
public:
    GetComponentsJob( QCoreApplication* app, QObject* obj, const orcaice::Context& context, const std::string& locatorString );

    virtual void execute();
    virtual std::string toString() const { return "GetComponentsJob"; };

private:
    QCoreApplication* app_;
    QObject* obj_;
    std::string locatorString_;
    orcaice::Context context_;
};


} // namespace

#endif

