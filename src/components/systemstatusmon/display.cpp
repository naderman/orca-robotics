/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#include <hydrocolourtext/colourtext.h>
#include <orcasystemstatusutil/orcasystemstatusutil.h>
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

hydroctext::Style toStyle( const orca::ComponentHealth& health )
{
    switch( health )
    {
        case orca::CompOk:
            return hydroctext::Style( hydroctext::Reverse, hydroctext::Green );
        case orca::CompWarning:  
            return hydroctext::Style( hydroctext::Reverse, hydroctext::Yellow );
        case orca::CompFault:  
            return hydroctext::Style( hydroctext::Reverse, hydroctext::Red );
        default:
            assert( !"Unknown component health." );
    }
}

hydroctext::Style toStyle( const orca::ComponentHealth& health, bool isStalled )
{
    if ( isStalled )
        return hydroctext::Style( hydroctext::Reverse, hydroctext::Black );
    else
        return toStyle( health );
}

void toStyle( const orca::EstimatedComponentStatus& estStatus, hydroctext::Style& compStyle, 
            string& compIcon, string& estCompIcon )
{
    switch( estStatus.estimatedState )
    {
        case orca::EstCompInactive :
        {
            compIcon = " ";
            compStyle = hydroctext::Style( hydroctext::Reverse, hydroctext::White );
            estCompIcon = hydroctext::emph(" ", compStyle ); 
            break;
        }
        case orca::EstCompReporting :
        {
            if ( estStatus.reportedStatus.empty() )
                throw gbxutilacfr::Exception( ERROR_INFO, "Empty reported status in EstCompReporting state." );
            compIcon = orcasystemstatusutil::toIcon( estStatus.reportedStatus[0].state );
            compStyle = toStyle( estStatus.reportedStatus[0].health, estStatus.reportedStatus[0].isStalled );
            estCompIcon = hydroctext::emph(" ", compStyle ); 
            break;
        }
        case orca::EstCompStale :
        {
            if ( estStatus.reportedStatus.empty() )
                throw gbxutilacfr::Exception( ERROR_INFO, "Empty reported status in EstCompStale state." );
            compIcon = orcasystemstatusutil::toIcon( estStatus.reportedStatus[0].state );
            compStyle = toStyle( estStatus.reportedStatus[0].health, estStatus.reportedStatus[0].isStalled );
            estCompIcon = hydroctext::emph("!", hydroctext::Style( hydroctext::Reverse, hydroctext::Black ) ); 
            break;
        }
        default :
            assert( !"Unknown estimated component state." );
    }
}

std::string toShortString( const orca::EstimatedComponentStatus& estStatus, int stateWidth )
{   
    hydroctext::Style compStyle;
    string compIcon = "?";
    string estCompIcon = "?";
    toStyle( estStatus, compStyle, compIcon, estCompIcon );
    
    stringstream ss;
    int stateUsedWidth = 2;
    string compItem = compIcon + " " + estStatus.name.component;
    ss << estCompIcon
       << hydroctext::emph(hydroctext::toFixedWidth(compItem,stateWidth-stateUsedWidth), compStyle );
    
    return ss.str();
}

std::string toBoldString( const string &input )
{
    return hydroctext::emph(input, hydroctext::Style( hydroctext::Bold ) );
}

std::string humanMsgString( const orca::SystemStatusData &ssData )
{
    stringstream ss;
    
    orca::SystemStatusDict::const_iterator itCs;
    for ( itCs=ssData.systemStatus.begin(); itCs!=ssData.systemStatus.end(); ++itCs )
    {
        const orca::EstimatedComponentStatusSeq &components = itCs->second;
        
        // per component
        for (unsigned int i=0; i<components.size(); i++)
        {
            string platComp = orcaice::toString(components[i].name);
            if ( !components[i].reportedStatus.empty() )
            {
                const orca::SubsystemStatusDict &subSysSt = components[i].reportedStatus[0].subsystems;
                orca::SubsystemStatusDict::const_iterator itSs;
                
                // per subsystem
                for ( itSs=subSysSt.begin(); itSs!=subSysSt.end(); ++itSs )
                {
                    if ( !itSs->second.message.empty() )
                        ss << toBoldString(platComp + ", " + itSs->first + ": ") << endl
                            << itSs->second.message << endl << endl;
                }
            }
            else {
                // TODO: ???
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
    
    orca::SystemStatusDict::const_iterator it;
    
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
    int maxPlatformSize = orcasystemstatusutil::maxPlatformSize( data );
    
    for ( int compCounter=0; compCounter<maxPlatformSize; ++compCounter )
    {
        // row label
        if ( compCounter==0 )
            cout << hydroctext::toFixedWidth( toBoldString("COMPONENTS"), compNameWidth ) << sep;
        else
            cout << hydroctext::toFixedWidth( " ", compNameWidth ) << sep;
        
        // per platform (column)
        for ( it=data.systemStatus.begin(); it!=data.systemStatus.end(); ++it )
        {
            const orca::EstimatedComponentStatusSeq &components = it->second;
            if ( compCounter < (int)components.size() )
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
