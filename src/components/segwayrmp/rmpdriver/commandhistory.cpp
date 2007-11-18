#include "commandhistory.h"
#include <iostream>

using namespace std;

namespace segwayrmp {

CommandHistory::CommandHistory( int historyLength )
    : historyLength_(historyLength)
{
}

void
CommandHistory::setCommand( int16_t speed, int16_t turnrate )
{
    if ( (int)(sentSpeeds_.size()) == historyLength_ )
    {
        sentSpeeds_.pop_front();
        sentTurnrates_.pop_front();
    }

    sentSpeeds_.push_back( speed );
    sentTurnrates_.push_back( turnrate );
}

void
CommandHistory::receivedCommandWasRecentlySent( int16_t rxSpeed, int16_t rxTurnrate,
                                                bool speedRecentlySent, bool turnrateRecentlySent ) const
{
    speedRecentlySent    = false;
    turnrateRecentlySent = false;

    for ( std::deque<int16_t>::const_iterator it = sentSpeeds_.begin();
          it != sentSpeeds_.end();
          it++ )
    {
        if ( *it == rxSpeed )
        {
            speedRecentlySent = true;
            break;
        }
    }

    for ( std::deque<int16_t>::const_iterator it = sentTurnrates_.begin();
          it != sentTurnrates_.end();
          it++ )
    {
        if ( *it == rxTurnrate )
        {
            turnrateRecentlySent = true;
            break;
        }
    }          
}

}

