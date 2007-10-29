#include "interactivecontroller.h"
#include <iostream>
#include <hydroutil/tokenise.h>

using namespace std;

namespace logplayer {

namespace {

    void printMenu()
    {
        cout << "  p)ause              : toggle playing"  << endl
             << "  r)eplay-rate <rate> : set replay rate" << endl
             << "  q)uit" << endl;
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
        cout << " =>";

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
        cout<<"TRACE(interactivecontroller.cpp): num: " << num << endl;
        cout<<"TRACE(interactivecontroller.cpp): newRate: " << newRate << endl;
        if ( num != 1 )
        {
            printMenu();
            return;
        }

        replayConductor_.setReplayRate( newRate );
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

