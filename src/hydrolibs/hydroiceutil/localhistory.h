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

    // from hydroutil::History
    virtual void set( const std::string &message );
    virtual void setWithStartSequence( const std::string& message );
    virtual void setWithFinishSequence( const std::string& message );
    virtual void autoStart( bool force=false );
    virtual void autoFinish( bool force=false );
    virtual void flush();

protected:

    // We only have one communicator but may have multiple threads.
    IceUtil::Mutex mutex_;

private:

    bool enabled_;
    std::string message_;

    std::auto_ptr<std::ofstream> file_;

    hydroutil::Properties properties_;
};

} // namespace

#endif
