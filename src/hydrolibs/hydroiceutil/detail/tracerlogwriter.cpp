#include "tracerlogwriter.h"
#include <iostream>

using namespace std;

namespace hydroiceutil {
namespace detail {

namespace {
    const int    MAX_REPEAT_COUNT = 100;
    const double MAX_SILENCE_SEC  = 300;
}

TracerLogWriter::TracerLogWriter( const std::string &compName, bool isApp )
    : sysLogger_(compName,isApp)
{
}

void
TracerLogWriter::log( SysLogger::LogPriority  priority,
                      const std::string      &msg,
                      const std::string      &subsystem )
{
    std::map<std::string,LogCompressor>::iterator it = logCompressors_.find( subsystem );
    if ( it == logCompressors_.end() )
    {
        LogCompressor newLogCompressor( MAX_REPEAT_COUNT, MAX_SILENCE_SEC );
        logCompressors_.insert( 
            std::pair<std::string,LogCompressor>( subsystem, newLogCompressor ) );
        // TODO: is there a way we can get around re-finding ??
        it = logCompressors_.find( subsystem );
    }

    std::string completeMessage = toPriorityString(priority)+": "+msg;
    
    bool shouldWriteInFull;
    bool shouldWriteRepeatMessage;
    int  numRepeats;
    string repeatedMessage;
    it->second.newMsgGenerated( completeMessage,
                                shouldWriteInFull,
                                shouldWriteRepeatMessage,
                                numRepeats,
                                repeatedMessage );

    if ( shouldWriteRepeatMessage )
    {
        std::stringstream repeatMsgSS;
        repeatMsgSS << subsystem << ": ";
        repeatMsgSS << "Last message ";
        if ( shouldWriteInFull )
        {
            // Use past tense to note that we're clearing a backlog
            repeatMsgSS << "was ";
        }
        repeatMsgSS << "repeated " << numRepeats << " times: " << repeatedMessage;
        sysLogger_.log( priority, repeatMsgSS.str() );
    }
    if ( shouldWriteInFull )
    {
        std::string preamble;
        if ( subsystem != "" )
            preamble = subsystem + ": ";
        sysLogger_.log( priority, preamble+completeMessage );
    }
}

}
}
