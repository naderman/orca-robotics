/*
 * Orca-Robotics Project: Components for robotics
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef HYDROICEUTIL_LOCAL_HISTORY_H
#define HYDROICEUTIL_LOCAL_HISTORY_H

#include <hydroutil/history.h>
#include <hydroutil/properties.h>
#include <hydroutil/uncopyable.h>
#include <gbxsickacfr/gbxiceutilacfr/timer.h>

#include <IceUtil/Mutex.h>
#include <memory>

namespace hydroiceutil
{

/*!
@brief Writes component's run history to a local file.

Properties:
- @c "Enable" (bool)
    - Enables writing history to file.
    - Default: 0
- @c "Dir" (string)
    - If Path is not provided, specifies the directory where to store the history file with the default name.
    - Default: "./"
- @c "DefaultFilename" (string)
    - Default: "xxxx-history.txt"
- @c "Path" (string)
    - If given, specifies the full path to the history file.
    - Default: "${Dir}/${DefaultFilename}"
- @c "AutoSaveInterval" (int)
    - The mimimum interval between flushing history to file.
      The history will be written to file when a) setProgressMessage() is called, and b)
      the elapsed time since last write is greater than the AutoSaveInterval.
    - Units [sec]
    - Valid values: positive for auto save, negative to disable autosave.
    - Default: 60

If Enabled, tries to open the history file for writing. Throws hydroiceutil::Exception
if fails to open the file.

If not Enabled, does nothing.
*/
class LocalHistory : public hydroutil::History,
                    public hydroutil::Uncopyable
{
public:
    //! Constructor with optional properties.
    LocalHistory( const hydroutil::Properties& props=hydroutil::Properties() );
    ~LocalHistory();

    // from hydroutil::History
    virtual void report( const std::string& message="" );
    virtual bool isEnabled() const { return enabled_; };
    virtual int autoSaveInterval() const { return autoSaveInterval_; };

private:

    virtual void flush();
    virtual void maybeFlush();

    // these are not touched after the constructor (no need to protect them)
    bool enabled_;
    int autoSaveInterval_;

    IceUtil::Mutex mutex_;

    std::string message_;
    std::auto_ptr<std::fstream> file_;
    // the starting write position in the history file.
    std::streampos startPos_;
    // line width used for history (it is the maximum length of all progress
    // reports so far)
    int lineWidth_;

    gbxiceutilacfr::Timer runTimer_;
    gbxiceutilacfr::Timer flushTimer_;

    // currently not using these
    hydroutil::Properties properties_;
};

} // namespace

#endif
