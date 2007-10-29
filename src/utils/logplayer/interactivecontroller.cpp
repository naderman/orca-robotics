#include "interactivecontroller.h"
#include <iostream>
#include <hydroutil/tokenise.h>

using namespace std;

namespace logplayer {

namespace {

    void printMenu()
    {
        cout << "  (p)ause              : toggle playing"  << endl
             << "  (r)eplay-rate <rate> : set replay rate" << endl
             << "  re(w)ind [sec[:usec]]  : rewind (no arg = 'to start')" << endl
             << "  (q)uit" << endl;
    }

    // timeString should have format 'sec:usec' or format 'sec'
    bool
    parseTime( const std::string &timeString, IceUtil::Time &t )
    {
        int sec, usec;

        std::vector<string> tokens = hydroutil::tokenise(timeString,":");
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
    : replayConductor_(replayConductor),
      autoStart_(autoStart),
      context_(context)
{
}

void
InteractiveController::walk()
{
    printMenu();

    while ( !isStopping() )
    {
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
    std::vector<string> tokens = hydroutil::tokenise( input, " " );

    cout<<"TRACE(interactivecontroller.cpp): input: " << input << endl;
    cout<<"TRACE(interactivecontroller.cpp): numTokens: " << tokens.size() << endl;
    for ( uint i=0; i < tokens.size(); i++ )
    {
        cout<<"TRACE(interactivecontroller.cpp): tokens["<<i<<"]: '" << tokens[i] << "'" << endl;
    }

    if ( tokens.size() == 0 )
    {
        printMenu();
        return;
    }

    if ( tokens[0][0] == 'p' )
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
    else if ( tokens[0][0] == 'r' )
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
    else if ( tokens[0][0] == 'w' )
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
            IceUtil::Time rewindTime;
            bool timeOK = parseTime( tokens[1], rewindTime );
            if ( !timeOK )
            {
                cout<<"TRACE(interactivecontroller.cpp): badly formed time." << endl;
                printMenu();
                return;
            }
            replayConductor_.rewind( rewindTime );
        }
    }
    else if ( tokens[0][0] == 'q' )
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

