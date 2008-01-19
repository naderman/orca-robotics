/*c
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAQCM_JOBS_H
#define ORCAQCM_JOBS_H

#include <hydroiceutil/jobqueue.h>
#include <orcaice/context.h>
#include <QCoreApplication>

namespace orcaqcm
{

//! This job does the following:
//! - contacts the Registry described by @c locatorString, 
//! - gets Home interface data and 
//! - returns results by posting a HomeEvent (a QEvent) to be processed by an instance of OcmModel.
//!
//! Currently does not return any error information, probably should.
class GetComponentsJob : public hydroiceutil::Job
{
public:
    //! Contstructor
    GetComponentsJob( QCoreApplication* app, QObject* ocmModel, const orcaice::Context& context, const std::string& locatorString );

    virtual void execute();
    virtual std::string toString() const { return "GetComponentsJob"; };

private:
    QCoreApplication* app_;
    QObject* ocmModel_;
    std::string locatorString_;
    orcaice::Context context_;
};


} // namespace

#endif

