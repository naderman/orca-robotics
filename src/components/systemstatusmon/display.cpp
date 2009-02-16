/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#include <gbxutilacfr/tokenise.h>
#include <hydrocolourtext/colourtext.h>
#include <orcaobj/stringutils.h>
#include "display.h"

using namespace std;
using namespace systemstatusmon;      

namespace 
{  

std::string toHorizontalLine( int platformCount, int nameWidth, int stateWidth )
{
    stringstream ss;
    ss << hydroctext::toFixedWidth("-----------------------------------", nameWidth) << " +";
    for ( int i=0; i<platformCount; ++i )
        ss << hydroctext::toFixedWidth("-------------------------------------", stateWidth) << " +";
    return ss.str();
}

vector<string> extractPlatComp( const string &input )
{
    vector<string> tokens = gbxutilacfr::tokenise( input.c_str(), "/");
    if (tokens.size()!=2)
    {
        stringstream ss;
        ss << "extractPlatForm: unexpected <platform/component> string: " << input;
        throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
    }
    return tokens;
}

string extractPlatform( const string &input )
{
    vector<string> tokens = extractPlatComp( input );
    return tokens[0];
}

string extractComponent( const string &input )
{
    vector<string> tokens = extractPlatComp( input );
    return tokens[1];
}

unsigned int extractMaxNumComponents( const orca::SystemStatusData &data )
{
    unsigned int maxNum=0;
    map<string,vector<orca::ObservedComponentStatus> >::const_iterator it;
    
    for ( it=data.systemStatus.begin(); it!=data.systemStatus.end(); ++it )
    {
        if ( (it->second.size()) > maxNum )
            maxNum = it->second.size();
    }
    
    return maxNum;
}
    
std::string stateToString( const orca::ObservedComponentState &state )
{
    switch (state)
    {
        case orca::ObsCompInactive: 
            return "! ";
        case orca::ObsCompConnecting:
            return "^ ";
        case orca::ObsCompInitialising:
            return "- ";
        case orca::ObsCompActive:
            return "  ";
        case orca::ObsCompFinalising:
            return "v ";
        default:
            assert( false && "unknown state type" );
    }
}

void extractStateAndHealth( const orca::ObservedComponentStatus &compData,
                            hydroctext::Style                   &healthStyle,
                            string                              &stateIcon )
{   
    //
    // handle two special cases first
    //
    if ( (compData.state == orca::ObsCompInactive) || (compData.state == orca::ObsCompFinalising) )
    {
        stateIcon = hydroctext::emph( stateToString(compData.state), hydroctext::Style( hydroctext::Reverse, hydroctext::White ) );
        healthStyle = hydroctext::Style( hydroctext::Reverse, hydroctext::White );
        return;
    }
    
    switch (compData.health)
    {
        case orca::ObsCompOk:
            healthStyle = hydroctext::Style( hydroctext::Reverse, hydroctext::Green );
            stateIcon = hydroctext::emph( stateToString(compData.state), hydroctext::Style( hydroctext::Reverse, hydroctext::Green ) );
            return;
        case orca::ObsCompWarning:  
            healthStyle = hydroctext::Style( hydroctext::Reverse, hydroctext::Yellow );
            stateIcon = hydroctext::emph( stateToString(compData.state), hydroctext::Style( hydroctext::Reverse, hydroctext::Yellow ) );
            return;
        case orca::ObsCompFault:  
            healthStyle = hydroctext::Style( hydroctext::Reverse, hydroctext::Red );
            stateIcon = hydroctext::emph( stateToString(compData.state), hydroctext::Style( hydroctext::Reverse, hydroctext::Red ) );
            return;
        case orca::ObsCompStalled:  
            healthStyle = hydroctext::Style( hydroctext::Reverse, hydroctext::Black );
            stateIcon = hydroctext::emph( stateToString(compData.state), hydroctext::Style( hydroctext::Reverse, hydroctext::Black ) );
            return;
        case orca::ObsCompStale:  
            healthStyle = hydroctext::Style( hydroctext::Reverse, hydroctext::White );
            stateIcon = hydroctext::emph( stateToString(compData.state), hydroctext::Style( hydroctext::Reverse, hydroctext::White ) );
            return;
        default:
            assert( false && "unknown health type" );
    }
}

std::string toShortString( const orca::ObservedComponentStatus& compData, int stateWidth )
{   
    hydroctext::Style healthStyle;
    string stateIcon;
    extractStateAndHealth( compData, healthStyle, stateIcon );
    
    stringstream ss;
    int stateUsedWidth = 2;
    string compPlat = orcaobj::toString(compData.name);
    ss << stateIcon
            << hydroctext::emph(hydroctext::toFixedWidth(extractComponent(compPlat),stateWidth-stateUsedWidth), healthStyle );
    
    return ss.str();
}

std::string toBoldString( const string &input )
{
    return hydroctext::emph(input, hydroctext::Style( hydroctext::Bold ) );
}

std::string humanMsgString( const orca::SystemStatusData &ssData )
{
    stringstream ss;
    
    map<string,vector<orca::ObservedComponentStatus> >::const_iterator itCs;
    for ( itCs=ssData.systemStatus.begin(); itCs!=ssData.systemStatus.end(); ++itCs )
    {
        const vector<orca::ObservedComponentStatus> &components = itCs->second;
        
        // per component
        for (unsigned int i=0; i<components.size(); i++)
        {
            string platComp = orcaobj::toString(components[i].name);   
            const orca::SubsystemStatusDict &subSysSt = components[i].subsystems;
            map<string,orca::SubsystemStatus>::const_iterator itSs;
            
            // per subsystem
            for ( itSs=subSysSt.begin(); itSs!=subSysSt.end(); ++itSs )
            {
                if (itSs->second.message!="")
                    ss << toBoldString(platComp + ", " + itSs->first + ": ") << endl
                        << itSs->second.message << endl << endl;
            }
        }
    }
    
    return ss.str();
}

} // end of namespace
   
////////////////////////////////////// 

ColourTextDisplay::ColourTextDisplay( const orcaice::Context   &context,
                                      gbxiceutilacfr::Thread *thread )
    : context_(context),
      publishIntervalSec_(10.0)
{
    consumer_ = new orcaifaceimpl::BufferedSystemStatusConsumerImpl( -1, gbxiceutilacfr::BufferTypeCircular, context );
    consumer_->subscribeWithTag( "SystemStatus", thread );
    timeSinceHeardTimer_.restart();
}

void 
ColourTextDisplay::refresh()
{
    if (consumer_->buffer().isEmpty()) 
    {
        context_.tracer().debug( "ColourTextDisplay:refresh(): systemstatus buffer is empty", 5 );
        
        if ( timeSinceHeardTimer_.elapsedSec() > publishIntervalSec_ )
        {
            stringstream ss;
            ss << "Haven't received SystemStatusData for " << timeSinceHeardTimer_.elapsedSec() << " seconds. SystemStatus may be dead.";
            cout << hydroctext::emph( ss.str(), hydroctext::Style( hydroctext::Reverse, hydroctext::Red ) ) << endl << endl;
        }
        return;
    }
    
    // start the timer
    timeSinceHeardTimer_.restart();
        
    orca::SystemStatusData data;
    consumer_->buffer().getAndPop( data );
    
    try 
    {
        display( data );
    }
    catch (std::exception &e)
    {
        stringstream ss;
        ss << "ColourTextDisplay: Caught unexpected exception: " << e.what();
        context_.tracer().error( ss.str() );
    }
}
    
void
ColourTextDisplay::display( const orca::SystemStatusData &data )
{   
       
    publishIntervalSec_ = data.publishIntervalSec;
    
    map<string,vector<orca::ObservedComponentStatus> >::const_iterator it;
    
    const string sep = " |";
    const int compNameWidth = 20;
    const int compStateWidth = 22;   

    // start with "clean" colour settings
    cout << hydroctext::reset();  
    
    // platform names
    cout << hydroctext::toFixedWidth( toBoldString("PLATFORMS"), compNameWidth ) << sep;
    for ( it=data.systemStatus.begin(); it!=data.systemStatus.end(); ++it )
    {
        cout << hydroctext::toFixedWidth( "    " + toBoldString(it->first), compStateWidth ) << sep;
    }
    cout << endl;
    
    // horizontal line
    cout << toHorizontalLine(data.systemStatus.size(),compNameWidth,compStateWidth) << endl;
    
    // find maximum number of components per platform -> number of rows
    unsigned int maxComp = extractMaxNumComponents( data );
    
    for (unsigned int compCounter=0; compCounter<maxComp; compCounter++)
    {
        // row label
        if (compCounter==0)
            cout << hydroctext::toFixedWidth( toBoldString("COMPONENTS"), compNameWidth ) << sep;
        else
            cout << hydroctext::toFixedWidth( " ", compNameWidth ) << sep;
        
        // per platform (column)
        for ( it=data.systemStatus.begin(); it!=data.systemStatus.end(); ++it )
        {
            const vector<orca::ObservedComponentStatus> &components = it->second;
            if ( compCounter < components.size() )
                cout << toShortString(components[compCounter],compStateWidth) << sep;
            else
                cout << hydroctext::toFixedWidth( " ", compStateWidth ) << sep;
        }
        cout << endl;
    }
    
    // print the human-readable text
    cout << endl;
    cout << humanMsgString( data );
    
    // space between consecutive records
    cout << endl << endl;
}
