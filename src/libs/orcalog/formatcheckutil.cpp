#include "formatcheckutil.h"
#include <iostream>
#include "exceptions.h"

using namespace std;

namespace orcalog {

void checkFormats( const orcalog::LogReaderInfo   &logReaderInfo,
                   const std::vector<std::string> &okFormats )
{
    for ( size_t i=0; i < okFormats.size(); i++ )
    {
        if ( logReaderInfo.format == okFormats[i] )
        {
            // format is supported
            return;
        }
    }

    // format is not supported: throw exception
    stringstream ss;
    ss << logReaderInfo.interfaceName<<": unknown log format: "<<logReaderInfo.format<<endl
       << "  Supported formats are: ";
    for ( size_t i=0; i < okFormats.size(); i++ )
    {
        ss << okFormats[i];
        if ( i != 0 ) ss << ", ";
    }
    throw orcalog::FormatNotSupportedException( ERROR_INFO, ss.str() );
}

void
checkFormatDefaultsOnly( const orcalog::LogReaderInfo &logReaderInfo )
{
    vector<string> okFormats;
    okFormats.push_back("ice");
    okFormats.push_back("asciigenerated");
    checkFormats( logReaderInfo, okFormats );
}

void
checkFormatDefaultsOrAscii( const orcalog::LogReaderInfo &logReaderInfo )
{
    vector<string> okFormats;
    okFormats.push_back("ice");
    okFormats.push_back("ascii");
    okFormats.push_back("asciigenerated");
    checkFormats( logReaderInfo, okFormats );
}

void checkFormats( const std::string              &format,
                   const std::vector<std::string> &okFormats )
{
    for ( size_t i=0; i < okFormats.size(); i++ )
    {
        if ( format == okFormats[i] )
        {
            // format is supported
            return;
        }
    }

    // format is not supported: throw exception
    stringstream ss;
    ss << "Unknown log format: "<<format<<endl
       << "  Supported formats are: ";
    for ( size_t i=0; i < okFormats.size(); i++ )
    {
        ss << okFormats[i];
        if ( i != okFormats.size()-1 ) ss << ", ";
    }
    throw orcalog::FormatNotSupportedException( ERROR_INFO, ss.str() );
}

void
checkFormatDefaultsOnly( const std::string &format )
{
    vector<string> okFormats;
    okFormats.push_back("ice");
    okFormats.push_back("asciigenerated"); // auto-generated -> always available; need to adjust function name
    checkFormats( format, okFormats );
}

void
checkFormatDefaultsOrAscii( const std::string &format )
{
    vector<string> okFormats;
    okFormats.push_back("ice");
    okFormats.push_back("ascii");
    okFormats.push_back("asciigenerated");
    checkFormats( format, okFormats );
}

}

