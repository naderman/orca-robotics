#ifndef HYDROICEUTIL_LOGCOMPRESSOR_H
#define HYDROICEUTIL_LOGCOMPRESSOR_H

#include <hydroiceutil/detail/syslogger.h>
#include <gbxsickacfr/gbxiceutilacfr/timer.h>

namespace hydroiceutil {

namespace detail {

//
// @brief Watches for repeated messages, tells when to compress with e.g. "Last message repeated 543 times".
//
// @author Alex Brooks
//
class LogCompressor
{

public: 

    //
    // We recommend "Last message repeated XXX times" when either:
    //   - We've been silent for more than maxSilenceSec seconds, or
    //   - We've accumulated maxRepeatCount repeats.
    //
    LogCompressor( int maxRepeatCount, double maxSilenceSec );

    // Tells us what to write when a new message is generated.
    // It's possible for _both_ shouldWriteInFull and shouldWriteRepeatMessage to be set,
    // if there's a backlog of repeated messages when a new message is generated.
    //
    // If shouldWriteRepeatMessage is set, both numRepeats and
    // repeatedMessage will also be set.
    //
    void newMsgGenerated( const std::string &msg,
                          bool              &shouldWriteInFull,
                          bool              &shouldWriteRepeatMessage,
                          int               &numRepeats,
                          std::string       &repeatedMessage );

private: 

    gbxiceutilacfr::Timer lastWriteTimer_;
    std::string           lastMessage_;
    int                   repeatCount_;

    const int    maxRepeatCount_;
    const double maxSilenceSec_;
};

}
}

#endif
