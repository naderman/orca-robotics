#include "interactivecontroller.h"
#include <iostream>
#include <gbxutilacfr/tokenise.h>

using namespace std;

namespace logplayer {

namespace {

    // timeString should have format 'sec:usec' or format 'sec'
    bool
    parseTime( const std::string &timeString, IceUtil::Time &t )
    {
        int sec, usec;

        std::vector<string> tokens = gbxutilacfr::tokenise(timeString,":");
        if ( tokens.size() == 1 )
        {
            int num = sscanf( tokens[0].c_str(), "%d", &sec );
            if ( num != 1 ) return false;
            t = IceUtil::Time::seconds(sec);
            return true;
        }
        else if ( tokens.size() == 2 )
        {
            int num = sscanf( tokens[0].c_str(), "%d", &sec );
            if ( num != 1 ) return false;
            num = sscanf( tokens[1].c_str(), "%d", &usec );
            t = IceUtil::Time::seconds(sec) + IceUtil::Time::microSeconds(usec);
            return true;            
        }
        else
        {
            return false;
        }
    }

}

InteractiveController::InteractiveController( ReplayConductor        &replayConductor,
                                              bool                    autoStart,
                                              const orcaice::Context &context )
    : gbxiceutilacfr::SafeThread(context.tracer()),
      replayConductor_(replayConductor),
      autoStart_(autoStart),
      context_(context)
{
}

void 
InteractiveController::printMenu()
{
    if ( replayConductor_.isPlayingOrAboutToStart() )
    {
        cout << "    -- LogPlayer is PLAYING -- " << endl;
    }
    else
    {
        cout << "    -- LogPlayer is PAUSED -- " << endl;
    }
    cout << cursorInfo() << endl;

    cout << "  (p)lay/(p)ause               : toggle playing"  << endl
         << "  (r)eplay-rate <rate>         : set replay rate" << endl
         << "  re(w)ind [sec[:usec]]        : rewind (no arg = 'to start')" << endl
         << "  (f)ast-forward [sec[:usec]]  : fast-forward (no arg = 'to end')" << endl
         << "  (l),(j)                      : step backward/forward" << endl
         << "  (q)uit" << endl;
}

std::string
InteractiveController::cursorInfo()
{
    stringstream ss;

    int cursorSec, cursorUsec;
    if ( replayConductor_.getCursorTime(cursorSec,cursorUsec) )
    {
        int firstItemSec, firstItemUsec;
        replayConductor_.timeOfFirstItem( firstItemSec, firstItemUsec );
        IceUtil::Time timeInto = 
            orcalog::iceUtilTime(cursorSec,cursorUsec) -
            orcalog::iceUtilTime(firstItemSec,firstItemUsec);
        int intoSec = timeInto.toSeconds();
        int intoUsec = (int)(timeInto.toMicroSeconds()-timeInto.toSeconds()*1e6);

        ss << "cursorTime: " << cursorSec << ":" << cursorUsec << endl;
        ss << "            ("<<intoSec<<":"<<intoUsec<<" into log)";
    }
    else
    {
        ss << "cursor at end of log";
    }
    return ss.str();
}

void
InteractiveController::walk()
{
    if ( autoStart_ )
        replayConductor_.startPlaying();

    printMenu();

    while ( !isStopping() )
    {
        cout << cursorInfo() << endl;
        cout << " => ";

        const int NUM = 1000;
        char input[NUM];
        cin.getline(input,NUM);
        parseInput( input );
    }
}

void
InteractiveController::parseInput( const std::string &input )
{
    std::vector<string> tokens = gbxutilacfr::tokenise( input, " " );

    if ( tokens.size() == 0 )
    {
        printMenu();
        return;
    }

    if ( tokens[0][0] == 'p' ) // play/pause
    {
        if ( replayConductor_.isPlayingOrAboutToStart() )
        {
            replayConductor_.pausePlaying();
        }
        else
        {
            replayConductor_.startPlaying();
        }
    }
    else if ( tokens[0][0] == 'r' ) // replay-rate
    {
        if ( tokens.size() != 2 )
        {
            printMenu();
            return;
        }

        double newRate;
        int num = sscanf( tokens[1].c_str(), "%lf", &newRate );
        if ( num != 1 )
        {
            printMenu();
            return;
        }

        replayConductor_.setReplayRate( newRate );
    }
    else if ( tokens[0][0] == 'w' ) // rewind
    {
        if ( tokens.size() == 1 )
        {
            replayConductor_.rewindToStartAndStop();
        }
        else if ( tokens.size() != 2 )
        {
            printMenu();
        }
        else
        {
            IceUtil::Time time;
            bool timeOK = parseTime( tokens[1], time );
            if ( !timeOK )
            {
                cout<<"TRACE(interactivecontroller.cpp): badly formed time." << endl;
                printMenu();
                return;
            }
            replayConductor_.rewind( time );
        }
    }
    else if ( tokens[0][0] == 'f' ) // fast-forward
    {
        if ( tokens.size() == 1 )
        {
            replayConductor_.fastForwardToEnd();
        }
        else if ( tokens.size() != 2 )
        {
            printMenu();
        }
        else
        {
            IceUtil::Time time;
            bool timeOK = parseTime( tokens[1], time );
            if ( !timeOK )
            {
                cout<<"TRACE(interactivecontroller.cpp): badly formed time." << endl;
                printMenu();
                return;
            }
            replayConductor_.fastForward( time );
        }
    }
    else if ( tokens[0][0] == 'j' ) // step backward
    {
        replayConductor_.stepBackward();
    }
    else if ( tokens[0][0] == 'l' ) // step forward
    {
        replayConductor_.stepForward();
    }
    else if ( tokens[0][0] == 'q' ) // quit
    {
        context_.communicator()->destroy();
        stop();
    }
    else
    {
        printMenu();
    }
}


}

