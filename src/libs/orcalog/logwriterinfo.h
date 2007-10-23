#ifndef ORCALOG_LOGWRITERINFO_H
#define ORCALOG_LOGWRITERINFO_H

#include <string>
#include <orcaice/context.h>
#include <orcalog/masterfilewriter.h>

namespace orcalog {


//!
//! - interfaceType:    eg 'LaserScanner2d'
//! - interfaceTag:     The tag used in the config files
//!                     (By convention, it is formed by adding a sequential number
//!                      to the interface type.
//!                      E.g. Laser0, Laser1, ..., Laser12, etc.)
//! - comment:          something informative to note at the beginning of the master log (eg 'laserscanner2d@mango/laser2d')
//! - format:           How the data is stored to files
//!                     Standard types are "ice" (binary) and "ascii".
//!                     Derived slaves are free to invent their own encodings.
//! - filename:         filename where actual received data is stored.
//! - MasterFileWriter: A reference to the class that writes the master file.
//! - context:          The communicator is required for Ice streaming operations.
//!
class LogWriterInfo {
public:

    LogWriterInfo( MasterFileWriter &theMasterFileWriter,
                   const orcaice::Context &theContext )
        : masterFileWriter(theMasterFileWriter),
          context(theContext)
        {}

    std::string interfaceType;
    std::string interfaceTag;
    std::string comment;
    std::string format;
    std::string filename;
    MasterFileWriter &masterFileWriter;
    orcaice::Context context;
};

}

#endif
