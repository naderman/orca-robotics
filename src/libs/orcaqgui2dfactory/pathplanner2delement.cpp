/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <orcaice/orcaice.h>
#include <orcaobj/pathplanner2d.h>
#include <hydroqguipath/wptolerancesdialog.h>
#include <hydroqguielementutil/ihumanmanager.h>
#include "pathplanner2delement.h"

using namespace std;

namespace orcaqgui2d {

////////////////////////////////////////////////////////////////////////////////
void
PathPlannerTaskAnswerConsumer::setData(const ::orca::PathPlanner2dData& data, const ::Ice::Current& )
{
    std::cout << "INFO(pathplanner2dconsumerI.cpp): Received results: " << orcaobj::toVerboseString(data) << std::endl;
    if (data.result==orca::PathOk) return; 
    
    QString msg("Pathplanner could not compute path!\nReason is: ");
    
    if (data.result==orca::PathStartNotValid) 
        msg.append("Path Start not valid");    
    else if (data.result==orca::PathDestinationNotValid) 
        msg.append("Path Destination not valid");    
    else if (data.result==orca::PathDestinationUnreachable) 
        msg.append("Destination unreachable");    
    else if (data.result==orca::PathOtherError) 
        msg.append("OtherError");

    msg.append( ": " );
    msg.append( QString(data.resultDescription.c_str()) );

    msgStore_.set(msg); 
}

PathPlanner2dElement::PathPlanner2dElement( const hydroqguielementutil::GuiElementInfo &guiElementInfo,
                                            const orcaice::Context                     &context,
                                            const std::string                          &proxyString,
                                            hydroqguielementutil::IHumanManager        &humanManager,
                                            hydroqguielementutil::MouseEventManager    &mouseEventManager,
                                            hydroqguielementutil::ShortcutKeyManager   &shortcutKeyManager )
    : orcaqguielementutil::IceStormGuiElement2d<PathPlannerPainter,
                        orca::PathPlanner2dData,
                        orca::PathPlanner2dPrx,
                        orca::PathPlanner2dConsumer,
                        orca::PathPlanner2dConsumerPrx>( guiElementInfo, context, proxyString, painter_, -1 ),
      context_(context),
      proxyString_(proxyString),
      humanManager_(humanManager),
      pathHI_( this,
               proxyString,
               humanManager,
               shortcutKeyManager,
               mouseEventManager,
               painter_,
               context_ )
{
//     cout<<"TRACE(pathplanner2delement.cpp): Instantiating w/ proxyString '" << proxyString << "'" << endl;   
    const bool useTransparency = true;
    const bool displayOlympicMarker = true;
    painter_.initialize( useTransparency, displayOlympicMarker );
    pathTaskAnswerConsumer_ = new PathPlannerTaskAnswerConsumer;
}

PathPlanner2dElement::~PathPlanner2dElement()
{
}

void
PathPlanner2dElement::update()
{    
     if ( !needToUpdate() ) {
        return;
    }
    
    listener_.store().get( data_ );
    painter_.setData( data_ );
    
    if ( pathTaskAnswerConsumer_->msgStore_.isNewData() )
    {
        QString msg;
        pathTaskAnswerConsumer_->msgStore_.get( msg );
        humanManager_.showDialogError( msg );    
    }
}

void 
PathPlanner2dElement::setUseTransparency( bool useTransparency ) 
{ 
//     cout << "TRACE(pathplanner2delement.cpp): setUseTransparency: " << useTransparency << endl;
    painter_.setUseTransparency( useTransparency ); 
    pathHI_.setUseTransparency( useTransparency );
}

void
PathPlanner2dElement::actionOnConnection()
{
    humanManager_.showStatusInformation( "PathplannerElement is trying to connect" );
     
    try 
    {
        orcaice::connectToInterfaceWithString( context_, pathPlanner2dPrx_, proxyString_ );
    }
    catch ( ... )
    {
        humanManager_.showStatusWarning( "Problem connecting to pathplanner interface. Will try again later.");
        //cout << "WARNING(pathplanner2delement.cpp): Problem connecting to interface. Will try again later." << endl;
        return;
    }
    humanManager_.showStatusInformation( "Connected to pathplanner interface successfully." );
    
    pathPlanner2dConsumerObj_ = pathTaskAnswerConsumer_;
    taskCallbackPrx_ = orcaice::createConsumerInterface<orca::PathPlanner2dConsumerPrx>( context_,
                                                                                         pathPlanner2dConsumerObj_ );
}

QStringList
PathPlanner2dElement::contextMenu()
{
    QStringList s;
    if (painter_.displayWaypoints()) {
        s << "Switch all waypoints OFF";
    } else {
        s << "Switch all waypoints ON";
    }
    if (painter_.useTransparency()) {
        s << "Switch transparency OFF";
    } else {
        s << "Switch transparency ON";
    }
    
    s  << "Save path as..."
       << "Save path";
    return s;
}

void 
PathPlanner2dElement::execute( int action )
{
//     cout<<"TRACE(pathplanner2delement.cpp): execute: " << action << endl;
    if ( action == 0 )
        painter_.toggleDisplayWaypoints();    
    else if ( action == 1 )
        setUseTransparency(!painter_.useTransparency());
    else if ( action == 2 )
        pathHI_.savePathAs();
    else if ( action == 3 )
        pathHI_.savePath();
    else
        assert( false && "PathPlanner2dElement::execute(): bad action" );
}

void 
PathPlanner2dElement::sendPath( const PathPlannerInput &pathInput )
{
    try
    {
        orca::PathPlanner2dTask task = pathInput.getTask();
        task.timeStamp = orcaice::getNow();
        task.prx = taskCallbackPrx_;
        pathPlanner2dPrx_->setTask( task );
    }
    catch ( const Ice::Exception &e )
    {
        stringstream ss;
        ss << "While trying to set a pathfollowing data: " << endl << e;
        humanManager_.showStatusError( ss.str().c_str() );
    }
}

void 
PathPlanner2dElement::paint( QPainter *p, int z )
{ 
    painter_.paint( p, z );
    pathHI_.paint( p );
}

void
PathPlanner2dElement::setFocus( bool inFocus ) 
{ 
    painter_.setFocus( inFocus );
    pathHI_.setFocus( inFocus );
}

}

