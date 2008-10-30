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

namespace systemstatusmon
{        
    
// TODO: copy&paste from sysadmin -> library?    
    
// truncates or pads the string to the specified length
std::string fixedWidth( const std::string &s, int w )
{
    const int ESCAPE = 0x1B;

    std::string padded;
    int numNonEscape=0;
    bool parsingEscape=false;
    for ( unsigned int i=0; i < s.size(); i++ )
    {
        if ( s[i] == ESCAPE )
            parsingEscape = true;

        if ( parsingEscape )
        {
            padded += s[i];
        }
        else if ( numNonEscape < w )
        {
            padded += s[i];
            numNonEscape++;
        }

        if ( s[i] == 'm' && parsingEscape )
        {
            parsingEscape = false;
        }
    }
// cout<<"TRACE(systemadminconsumerI.cpp): w="<<w<<", s: " << s << ", padded="<<padded<< ", num="<<num << endl;


    for ( int i=numNonEscape; i < w; i++ )
        padded += " ";

    return padded;
}

std::string fixedWidth( const double& d, int w )
{
    stringstream ss;
    ss << d;
    return fixedWidth( ss.str(), w );
}

std::string toHorizontalLine( int nodeCount, int nameWidth, int stateWidth, int debugWidth=0 )
{
    stringstream ss;
    ss << fixedWidth( "-----------------------------------", nameWidth ) << " +";
    for ( int i=0; i<nodeCount; ++i )
        ss << fixedWidth("-------------------------------------",stateWidth+debugWidth) << " +";
    return ss.str();
}

// ===================================================================

// TODO: other stuff which could go into a library

vector<string> extractTokens( const string &input )
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
    vector<string> tokens = extractTokens( input );
    return tokens[0];
}

string extractComponent( const string &input )
{
    vector<string> tokens = extractTokens( input );
    return tokens[1];
}

unsigned int extractMaxNumComponents( const orca::SystemStatusData &data )
{
    unsigned int maxNum=0;
    map<string,vector<orca::ComponentStatusData> >::const_iterator it;
    
    for ( it=data.begin(); it!=data.end(); ++it )
    {
        if ( (it->second.size()) > maxNum )
            maxNum = it->second.size();
    }
    
    return maxNum;
}

hydroctext::Style toStyle( const orca::StatusData &statusData )
{ 
    const orca::SubsystemsStatus &subSysSt = statusData.subsystems;
    map<string,orca::SubsystemStatus>::const_iterator it;

    orca::SubsystemHealth worstHealth = orca::SubsystemOk;           
    for (it=subSysSt.begin(); it!=subSysSt.end(); ++it)
    {
        if (it->second.health > worstHealth )
            worstHealth = it->second.health;
    }
    
    switch (worstHealth)
    {
        case orca::SubsystemOk: 
            return hydroctext::Style( hydroctext::Reverse, hydroctext::Green );
        case orca::SubsystemWarning:  
            return hydroctext::Style( hydroctext::Reverse, hydroctext::Yellow );
        case orca::SubsystemFault:  
            return hydroctext::Style( hydroctext::Reverse, hydroctext::Red );
        case orca::SubsystemStalled:  
            return hydroctext::Style( hydroctext::Reverse, hydroctext::Black );
        default:
            assert( false && "unknown health type" );
    }
}

std::string getSystemStateIcon( const orca::StatusData &statusData )
{
    const orca::SubsystemsStatus &subSysSt = statusData.subsystems;
    map<string,orca::SubsystemStatus>::const_iterator it;

    orca::SubsystemState highestState = orca::SubsystemIdle;           
    for (it=subSysSt.begin(); it!=subSysSt.end(); ++it)
    {
        if (it->second.state > highestState )
            highestState = it->second.state;
    }
    
    switch (highestState)
    {
        case orca::SubsystemIdle: 
            return hydroctext::emph( "^", hydroctext::Style( hydroctext::Reverse, hydroctext::Yellow ) );
        case orca::SubsystemInitialising:
            return hydroctext::emph( "^", hydroctext::Style( hydroctext::Reverse, hydroctext::Blue ) );
        case orca::SubsystemWorking:
            return hydroctext::emph( "-", hydroctext::Style( hydroctext::Reverse, hydroctext::Green ) );
        case orca::SubsystemFinalising:
            return hydroctext::emph( "v", hydroctext::Style( hydroctext::Reverse, hydroctext::Blue ) );
        case orca::SubsystemShutdown:
            return hydroctext::emph( "x", hydroctext::Style( hydroctext::Reverse, hydroctext::Red ) );
        default:
            assert( false && "unknown state type" );
    }
}

std::string toShortString( const orca::ComponentStatusData& compData, int stateWidth )
{
    string compPlat = orcaobj::toString(compData.data.name);
    
    stringstream ss;
    int stateUsedWidth = 1;
    
    ss << getSystemStateIcon(compData.data) << hydroctext::emph(fixedWidth(extractComponent(compPlat),stateWidth-stateUsedWidth), toStyle(compData.data) );
    
    return ss.str();
}

// ===================================================================

ColourTextDisplay::ColourTextDisplay( const orcaice::Context     &context,
                                      gbxiceutilacfr::SafeThread *thread)
{
    consumer_ = new orcaifaceimpl::BufferedSystemStatusConsumerImpl( -1, gbxiceutilacfr::BufferTypeCircular, context );
    consumer_->subscribeWithTag( "SystemStatus", thread );
}

void 
ColourTextDisplay::refresh()
{
    orca::SystemStatusData data;
    if ( !consumer_->buffer().isEmpty() )
    {
        consumer_->buffer().getAndPop( data );
//         cout << ifacestring::toString( data );
        display( data );
    }
    else
    {
//         cout << "Buffer is empty" << endl;
    }
}

    
void
ColourTextDisplay::display( const orca::SystemStatusData &data )
{   
    map<string,vector<orca::ComponentStatusData> >::const_iterator it;

    const string sep = " |";
    const int compNameWidth = 20;
    const int compStateWidth = 22;     
    
    // platform names
    cout << hydroctext::reset();
    cout << fixedWidth( " ", compNameWidth ) << sep;
    for ( it=data.begin(); it!=data.end(); ++it )
    {
        cout << fixedWidth( "    " + it->first, compStateWidth ) << sep;
    }
    cout << endl;
    
    // horizontal line
    cout << toHorizontalLine(data.size(),compNameWidth,compStateWidth) << endl;
    
    // find maximum number of components per platform -> number of lines in the display
    unsigned int maxComp = extractMaxNumComponents( data );
    
    for (unsigned int compCounter=0; compCounter<maxComp; compCounter++)
    {
        // per platform
        cout << fixedWidth( " ", compNameWidth ) << sep;
        for ( it=data.begin(); it!=data.end(); ++it )
        {
            const vector<orca::ComponentStatusData> &components = it->second;
            if ( compCounter >= components.size() )
                continue;
            
            cout << toShortString(components[compCounter],compStateWidth) << sep;
            
            // string compPlat = orcaobj::toString(components[compCounter].data.name);
            // cout << fixedWidth( "    " + extractComponent(compPlat), compStateWidth ) << sep;
            
        }
        cout << endl;
    }
    
}
        
}


        