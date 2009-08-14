#include "logcompressor.h"
#include <iostream>

using namespace std;

namespace hydroiceutil {
namespace detail {

LogCompressor::LogCompressor( int maxRepeatCount, double maxSilenceSec )
    : repeatCount_(0),
      maxRepeatCount_(maxRepeatCount),
      maxSilenceSec_(maxSilenceSec)
{
}

void
LogCompressor::newMsgGenerated( const std::string &msg,
                                bool              &shouldWriteInFull,
                                bool              &shouldWriteRepeatMessage,
                                int               &numRepeats,
                                std::string       &repeatedMessage )
{
    // Write it out in full if the message has changed
    shouldWriteInFull = ( lastMessage_ != msg );
    shouldWriteRepeatMessage = false;

    if ( shouldWriteInFull && repeatCount_ > 0 )
    {
        // We're writing a new message, but we have a backlog of un-reported repeated messages.
        numRepeats = repeatCount_;
        shouldWriteRepeatMessage = true;
    }

    if ( !shouldWriteInFull )
    {
        repeatCount_++;

        if ( lastWriteTimer_.elapsedSec() > maxSilenceSec_ ||
             repeatCount_ >= maxRepeatCount_ )
        {
            numRepeats = repeatCount_;
            shouldWriteRepeatMessage = true;
        }
    }

    if ( shouldWriteRepeatMessage )
        repeatedMessage = lastMessage_;        

    lastMessage_ = msg;
    if ( shouldWriteInFull || shouldWriteRepeatMessage )
    {
        repeatCount_ = 0;
        lastWriteTimer_.restart();
    }
}

}
}
