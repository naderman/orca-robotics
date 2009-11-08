#ifndef ORCALOG_FORMATCHECKUTIL_H
#define ORCALOG_FORMATCHECKUTIL_H

#include <orcalog/logreader.h>

namespace orcalog {

// Format-checking for readers
void checkFormats( const orcalog::LogReaderInfo   &logReaderInfo,
                   const std::vector<std::string> &okFormats );
void checkFormatDefaultsOnly( const orcalog::LogReaderInfo &logReaderInfo );
void checkFormatDefaultsOrAscii( const orcalog::LogReaderInfo &logReaderInfo );

// Format-checking for writers
void checkFormats( const std::string              &format,
                   const std::vector<std::string> &okFormats );
void checkFormatDefaultsOnly( const std::string &format );
void checkFormatDefaultsOrAscii( const std::string &format );

}

#endif
