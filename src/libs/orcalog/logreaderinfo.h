#ifndef ORCALOG_LOGREADERINFO_H
#define ORCALOG_LOGREADERINFO_H

#include <string>
#include <orcaice/context.h>
#include <orcalog/masterfilereader.h>

namespace orcalog {

//!
//! - interfaceType:    eg 'LaserScanner2d'
//! - interfaceName:    This is the name under which the interface will be registered for replay.
//! - format:           How the data is stored to files
//!                     Standard types are "ice" (binary) and "ascii".
//!                     Derived slaves are free to invent their own encodings.
//! - filename:         filename where actual received data is stored.
//! - context:          The communicator is required for Ice streaming operations.
//!
class LogReaderInfo {
public:

    LogReaderInfo( const orcaice::Context &theContext )
        : context(theContext)
        {}

    std::string interfaceType;
    std::string interfaceName;
    std::string format;
    std::string filename;
    orcaice::Context context;
};

}

#endif
