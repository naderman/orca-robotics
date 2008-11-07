/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <orcaice/orcaice.h>

#include "termncursesuser.h"
#include "inputthread.h"
#include "../events.h"
// super-dodgy but can't make cmake recongnize included dir's properly
#include "../termutil/termutils.h"

using namespace std;
using namespace tracermon;

enum ColorType
{
    DefaultColor=0,
    InfoColor,
    WarningColor,
    ErrorColor,
    DebugColor,
    OtherColor,
    StatusColor,
    MenuColor,
    MenuSelectColor,
    NumberOfColors
};

TermNcursesUser::TermNcursesUser( const orcaice::Context & context ) : 
    SafeThread(context.tracer()),
    events_(new hydroiceutil::EventQueue),
    context_(context)
{
}

TermNcursesUser::~TermNcursesUser()
{
    //  Clean up ther terminal after ourselves
    delwin(statuswin_);
    delwin(mainwin_);
    delwin(menuwin_);
    unpost_form(menuform_);
    free_form(menuform_);
    free_field(menufields_[0]);
    free_field(menufields_[1]); 
    endwin();

    // userMainThread_ is blocked on user input
    // the only way for it to realize that we want to stop is to give it some keyboard input.
    cout<< "Component is quitting but the UserMainThread is blocked waiting for user input." << endl;
    cout<< "************************************************" << endl;
    cout<< "Press any key to continue." << endl;
    cout<< "************************************************" << endl;
    gbxiceutilacfr::stopAndJoin( inputMainThread_ );
}

void 
TermNcursesUser::enable( Network* network )
{
    assert( network || "pointer to network must be non-zero" );
    if ( !network ) {
        cout<<"null network pointer. exitting..."<<endl;
        exit(1);
    }
    network_ = network;

    // Start curses mode 
    initscr();
    // Line buffering disabled
    cbreak();
    // Turn off key echoing
    noecho();
    // enable non-text input
    keypad( stdscr, true );

    assume_default_colors( COLOR_BLACK, COLOR_WHITE|A_BOLD );
    // Initialize colours
    start_color();
    // Make sure we are able to do what we want. If
    // has_colors() returns FALSE, we cannot use colours.
    // COLOR_PAIRS is the maximum number of colour pairs
    // we can use. We use 13 in this program, so we check
    // to make sure we have enough available.

    if ( !has_colors() && COLOR_PAIRS<NumberOfColors ) {
        // shouldn't happen
        cerr<<"Error initializing colors."<<endl;
        exit(1);
    }
    // init_pair(pair number, foreground, background);
    init_pair( InfoColor,    COLOR_BLACK,   -1 );
    init_pair( WarningColor, COLOR_BLUE,    -1 );
    init_pair( ErrorColor,   COLOR_RED,     -1 );
    init_pair( DebugColor,   COLOR_YELLOW,  -1 );
    init_pair( OtherColor,   COLOR_GREEN,   -1 );
    init_pair( StatusColor,  COLOR_WHITE,   COLOR_BLACK );
    init_pair( MenuColor, COLOR_WHITE,COLOR_BLACK );
    init_pair( MenuSelectColor,    COLOR_YELLOW,  COLOR_BLACK );
    refresh();

    statuswin_ = newwin(1, COLS, 0, 0);   
//     box(statuswin_, 0, 0);
//     keypad( statuswin_, true );     
    wcolor_set( statuswin_, 1, NULL );
    wbkgdset( statuswin_, COLOR_PAIR(StatusColor)|A_BOLD ); 
//     touchwin( statuswin_ );
    // nothing else seems to make the bitch repaint the whole window
    for ( int i=0; i<COLS; ++i ) {
        mvwaddch( statuswin_, 0, i, ' ' );
    }
    mvwprintw( statuswin_, 0, 4, "%s", "Orca2 TracerMon Component" );
    wrefresh( statuswin_ );

    mainwin_ = newwin(LINES-4, COLS, 1, 0);
//     idlok( mainwin_, true );
    scrollok( mainwin_, true );
//     box(mainwin_, 0, 0);
    keypad( mainwin_, true );        
    wbkgdset( mainwin_, COLOR_PAIR(DefaultColor) ); 
    touchwin( mainwin_);      
    wrefresh( mainwin_ );
//     wcolor_set( mainwin_, 1, NULL );   
//     wattron( mainwin_, A_REVERSE|A_BOLD );    
//     mvwprintw( mainwin_, 0, 4, "%s\n", "Orca2 TracerMon Component" );   
//     wattroff( mainwin_, A_REVERSE|A_BOLD );
    wrefresh( mainwin_ );

    menuwin_ = newwin(3, COLS, LINES-3, 0);   
    box(menuwin_, 0, 0);
    keypad( menuwin_, true );     
    wbkgdset( menuwin_, COLOR_PAIR(MenuColor) );
    wattron( menuwin_, A_BOLD );  
    for ( int j=0; j<LINES; ++j ) {
        for ( int i=0; i<COLS; ++i ) {
            mvwaddch( menuwin_, j, i, ' ' );
        }
    }

    // Initialize the fields (must be NULL-terminated)
    int ind = 0;
    for ( int i=0; i<3; ++i ) {
        for ( int j=0; j<5; ++j ) {
            ind = i*5+j;
            // height, width, starty, startx, number of offscreen rows and number of additional working buffers
            menufields_[ind] = new_field(1, 8, i, j*9, 0, 0);
            set_field_fore( menufields_[ind], COLOR_PAIR(MenuColor)|A_BOLD );
            set_field_back( menufields_[ind], COLOR_PAIR(MenuColor)|A_BOLD );
            if ( i==0 ) {
                set_field_just( menufields_[ind], JUSTIFY_CENTER );
            }
        }
    }
    menufields_[15] = NULL;

    set_field_buffer(menufields_[1], 0, "Error"); 
    set_field_buffer(menufields_[2], 0, "Warning"); 
    set_field_buffer(menufields_[3], 0, "Info"); 
    set_field_buffer(menufields_[4], 0, "Debug"); 
    set_field_buffer(menufields_[5], 0, "Current");
    set_field_buffer(menufields_[10],0, "New");  

    // initial states
    menuPos_ = 0;
    set_field_fore( menufields_[11+menuPos_], COLOR_PAIR(MenuSelectColor)|A_BOLD );
    set_field_back( menufields_[11+menuPos_], COLOR_PAIR(MenuSelectColor)|A_BOLD );

    for ( int i=0; i<4; ++i ) {
        // this should come from the remote object
        menuCurrState_[i] = 0;
        menuNewState_[i] = -1;
    }
    for ( int i=0; i<4; ++i ) {
        set_field_buffer(menufields_[6+i], 0, "0"); 
        set_field_buffer(menufields_[11+i], 0, "-1"); 
    }

    // Create the form and post it
    menuform_ = new_form(menufields_);
    set_form_win( menuform_, menuwin_ );
    post_form(menuform_);
    wrefresh( menuwin_ );

    // move cursor to status window so that cout dumps its shit there
    wmove( statuswin_, 0, 15 );
}

void 
TermNcursesUser::newTraceMessage( const orca::TracerData & data )
{
//     cout<<"TermNcursesUser::newTraceMessage"<<endl;
    hydroiceutil::EventPtr e = new NewTraceMessageEvent( data );
    events_->add( e );
}

void 
TermNcursesUser::previousField()
{
    hydroiceutil::EventPtr e = new PreviousFieldEvent;
    events_->add( e );
}

void 
TermNcursesUser::nextField()
{
    hydroiceutil::EventPtr e = new NextFieldEvent;
    events_->add( e );
}

void 
TermNcursesUser::valueUp()
{
    hydroiceutil::EventPtr e = new ValueUpEvent;
    events_->add( e );
}

void 
TermNcursesUser::valueDown()
{
    hydroiceutil::EventPtr e = new ValueDownEvent;
    events_->add( e );
}

void 
TermNcursesUser::action()
{
    hydroiceutil::EventPtr e = new ActionEvent;
    events_->add( e );
}

void 
TermNcursesUser::newVerbosityLevel( int error, int warn, int info, int debug )
{
//     cout<<"TermNcursesUser::newTraceMessage"<<endl;
    hydroiceutil::EventPtr e = new VerbosityLevelsChangedEvent( error, warn, info, debug );
    events_->add( e );
}

void 
TermNcursesUser::newLocalTrace( const std::string& msg )
{
    hydroiceutil::EventPtr e = new NewLocalTraceEvent( msg );
    events_->add( e );
}

// read commands from the keyboard. Launced in a separate thread.
void
TermNcursesUser::walk()
{
    // we are in a different thread now, catch all stray exceptions
    try
    {
    inputMainThread_ = new MainThread( *network_, *this, context_ );
    inputMainThread_->start();
    
    hydroiceutil::EventPtr event;
    int timeoutMs = 500;
    
    //
    // Main loop
    //
displayLocalTrace( "TermNcursesUser: entering main loop" );
    while ( !isStopping() )
    {
        if ( !events_->timedGet( event, timeoutMs ) ) {
            continue;
        }

        switch ( event->type() )
        {
        // approx in order of call frequency
        case NewTraceMessage : {
            //cout<<"focus changed event"<<endl;
            NewTraceMessageEventPtr e = NewTraceMessageEventPtr::dynamicCast( event );
            if ( !e ) {
                cout<<"failed to cast event to NewTraceMessage"<<endl;
                break;
            }
            displayTrace( e->data_ );
            break;
        }      
        case NewLocalTrace : {
            //cout<<"focus changed event"<<endl;
            NewLocalTraceEventPtr e = NewLocalTraceEventPtr::dynamicCast( event );
            if ( !e ) {
                cout<<"failed to cast event to NewLocalTrace"<<endl;
                break;
            }
            displayLocalTrace( e->msg_ );
            break;
        }    
        case NextField : {
//             set_field_fore( menufields_[11+menuPos_], A_NORMAL );
//             set_field_back( menufields_[11+menuPos_], A_NORMAL );
            set_field_fore( menufields_[11+menuPos_], COLOR_PAIR(MenuColor)|A_BOLD );
            set_field_back( menufields_[11+menuPos_], COLOR_PAIR(MenuColor)|A_BOLD );
            menuPos_++;
            if ( menuPos_>3 ) menuPos_=3;
//             set_field_fore( menufields_[11+menuPos_], A_REVERSE); 
//             set_field_back( menufields_[11+menuPos_], A_REVERSE); 
            set_field_fore( menufields_[11+menuPos_], COLOR_PAIR(MenuSelectColor)|A_BOLD );
            set_field_back( menufields_[11+menuPos_], COLOR_PAIR(MenuSelectColor)|A_BOLD );
            post_form(menuform_);
            wrefresh( menuwin_ );
            break;
        }
        case PreviousField : {
            set_field_fore( menufields_[11+menuPos_], COLOR_PAIR(MenuColor)|A_BOLD );
            set_field_back( menufields_[11+menuPos_], COLOR_PAIR(MenuColor)|A_BOLD );
            menuPos_--;
            if ( menuPos_<0 ) menuPos_=0;
            set_field_fore( menufields_[11+menuPos_], COLOR_PAIR(MenuSelectColor)|A_BOLD );
            set_field_back( menufields_[11+menuPos_], COLOR_PAIR(MenuSelectColor)|A_BOLD );
            post_form(menuform_);
            wrefresh( menuwin_ );
            break;
        }  
        case ValueUp : {
            menuNewState_[menuPos_]++;
            if ( menuNewState_[menuPos_]>99 ) menuNewState_[menuPos_]=99;
            stringstream ss;
            ss << menuNewState_[menuPos_];
            set_field_buffer( menufields_[11+menuPos_], 0, ss.str().c_str() ); 
            post_form(menuform_);
            wrefresh( menuwin_ );
            break;
        }  
        case ValueDown : {
            menuNewState_[menuPos_]--;
            if ( menuNewState_[menuPos_]<-1 ) menuNewState_[menuPos_]=-1;
            stringstream ss;
            ss << menuNewState_[menuPos_];
            set_field_buffer( menufields_[11+menuPos_], 0, ss.str().c_str() ); 
            post_form(menuform_);
            wrefresh( menuwin_ );
            break;
        }  
        case Action : {
// stringstream ss;
// ss<<"sending verbosity event with ["<<menuNewState_[0]<<","<<menuNewState_[1]<<","<<menuNewState_[2]<<","<<menuNewState_[3]<<"]";
// // displayLocalTrace( ss.str() );
// newLocalTrace( ss.str() );
// cout<<"DEBUG: testing with network"<<endl;
            network_->setVerbosityLevel( menuNewState_[0], menuNewState_[1], menuNewState_[2], menuNewState_[3] );
//     network_->callMeBackDirectly( events_ );
// displayLocalTrace( "done" );
            for ( int i=0; i<4; ++i ) {
                menuNewState_[i] = -1;
                set_field_buffer(menufields_[11+i], 0, "-1"); 
            }

            post_form(menuform_);
            wrefresh( menuwin_ );
            break;
        } 
        case VerbosityLevelsChanged : {
            //cout<<"focus changed event"<<endl;
            VerbosityLevelsChangedEventPtr e = VerbosityLevelsChangedEventPtr::dynamicCast( event );
            if ( !e ) {
                cout<<"failed to cast event to VerbosityLevelsChanged"<<endl;
                break;
            }
            menuCurrState_[0] = e->error_;
            menuCurrState_[1] = e->warn_;
            menuCurrState_[2] = e->info_;
            menuCurrState_[3] = e->debug_;

            for ( int i=0; i<3; ++i ) {
                stringstream ss;
                ss << menuCurrState_[i];
                set_field_buffer( menufields_[6+i], 0, ss.str().c_str() ); 
            }
            post_form(menuform_);
            wrefresh( menuwin_ );
            break;
        }        
        default : {
            cout<<"unknown display event "<<event->type()<<". Ignoring..."<<endl;
            break;
        }
        } // switch
    } // end of main loop

displayLocalTrace( "TermNcursesUser: exited main loop" );

    // reset the hardware
//     if ( driver_->disable() ) {
//         context_.tracer().warning("failed to disable driver");
//     }

    } // try
    catch ( ... )
    {
        context_.tracer().error( "unexpected exception from somewhere.");
        context_.communicator()->destroy();
    }

    // wait for the component to realize that we are quitting and tell us to stop.
    waitForStop();
    context_.tracer().debug( "TermNcursesUser: stopped.",5 );
}

void 
TermNcursesUser::displayTrace( const orca::TracerData& obj )
{
//     gbxutilacfr::TraceType type = gbxutilacfr::toTraceType( obj.category );
    ColorType color = DefaultColor;

    switch ( obj.type )
    {
    case orca::InfoTrace :
        color = DefaultColor;
//         wattron( mainwin_, COLOR_PAIR(DefaultColor) );
        break;
    case orca::WarningTrace :
        color = WarningColor;
//         wattron( mainwin_, COLOR_PAIR(WarningColor)|A_BOLD );
        break;
    case orca::ErrorTrace :
        color = ErrorColor;
//         wattron( mainwin_, COLOR_PAIR(ErrorColor)|A_BOLD );
        break;
    case orca::DebugTrace :
        color = DebugColor;
//         wattron( mainwin_, COLOR_PAIR(DebugColor)|A_BOLD );
        break;
    }

    wattron( mainwin_, COLOR_PAIR(color) );

//     mvwprintw( mainwin_, row_++, 0, "%s", tracermon::toString(obj).c_str() ); 
    wprintw( mainwin_, "%s\n", tracermon::toString(obj).c_str() );  

//     wcolor_set( mainwin_, DefaultColor, NULL );    
//     wattron( mainwin_, COLOR_PAIR(DefaultColor)|A_BOLD );
    wattroff( mainwin_, COLOR_PAIR(color)|A_BOLD );
    wrefresh( mainwin_ );

// cout<<tracermon::toString(obj)<<endl;
}

void 
TermNcursesUser::displayLocalTrace( const std::string& msg )
{
    wprintw( mainwin_, "LOCAL: %s\n", msg.c_str() );  
    wrefresh( mainwin_ );

// cout<<"LOCAL: "<<msg<<endl;
}
