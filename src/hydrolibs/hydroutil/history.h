/*
 * Orca-Robotics Project: Components for robotics
             http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef HYDROUTIL_HISTORY_H
#define HYDROUTIL_HISTORY_H

#include <string>

namespace hydroutil
{

//!
//! @brief Storage of historical information about component's activities.
//! The idea is to keep persistent record of component's startups and shutdowns.
//! A component can add arbitrary text information to the progress report.
//!
class History
{
public:

    virtual ~History() {};

    //! Writes a progress message.
    virtual void report( const std::string& message="" )=0;

    //! Is history enabled?
    virtual bool isEnabled() const=0;

    //! Retruns auto-save interval [s]
    virtual int autoSaveInterval() const=0;
};

} // namespace

#endif
