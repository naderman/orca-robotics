/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#ifndef ORCA_IPATH_INPUT_H
#define ORCA_IPATH_INPUT_H


namespace orcaqgui2d {

class WaypointSettings;
    
//!
//! Abstract class for user path input
//!
//! @author Tobias Kaupp
//!
class IPathInput
{             
    public:
        virtual ~IPathInput() {};  
     
        virtual void paint( QPainter *painter ) = 0;
        virtual void setUseTransparency( bool useTransparency ) = 0;
        
        virtual void processPressEvent( QMouseEvent* e) = 0;
        virtual void processReleaseEvent( QMouseEvent* e ) = 0;
        virtual void processDoubleClickEvent( QMouseEvent* e) = 0;
        virtual void processMoveEvent( QMouseEvent* e) = 0;
        
        virtual void updateWpSettings( WaypointSettings* wpSettings ) = 0;
};

}

#endif
