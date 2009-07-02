/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef PATHFOLLOWERPAINTER_H
#define PATHFOLLOWERPAINTER_H

#include <QColor>
#include <hydroqguipath/pathutils.h>
#include <hydroqguielementutil/definitions2d.h>
#include <gbxsickacfr/gbxiceutilacfr/timer.h>

// forward declarations
class QPainter;

namespace orcaqgui2d {

class WpColors;

struct PathFollowerPainterConfig
{
    PathFollowerPainterConfig()
    {
        displayWaypoints=true;
        displayPastWaypoints=true;
        displayFutureWaypoints=true;
        displayPathWhenFinished=true;
        displayPathWhenNotActivated=true;
        displayOlympicMarker=true;
        useTransparency=true;
    }

    void turnAllWpOff()
    {
        displayWaypoints=false;
        displayPastWaypoints=false;
        displayFutureWaypoints=false;
        displayPathWhenFinished=false;
        displayPathWhenNotActivated=false;
    }
    
    void turnAllWpOn()
    {
        displayWaypoints=true;
        displayPastWaypoints=true;
        displayFutureWaypoints=true;
        displayPathWhenFinished=true;
        displayPathWhenNotActivated=true;
    }
    
    bool displayWaypoints;
    bool displayPastWaypoints;
    bool displayFutureWaypoints;
    bool displayPathWhenFinished;
    bool displayPathWhenNotActivated;
    
    bool displayOlympicMarker;
    bool useTransparency;
};
    
class PathFollowerPainter
{

  public:
    PathFollowerPainter();
    
    void initialize( PathFollowerPainterConfig &config )
        { config_ = config; }
    
    void setData( const orca::PathFollower2dData& path );
    
    void setState( const orca::PathFollower2dState& state );

    void paint( QPainter *p, int z );
    
    bool paintThisLayer(int z) const 
        { return z==hydroqguielementutil::Z_PATH; }

    void clear()
        { guiPath_.resize(0); }
    
    void togglePastWaypoints()
        { config_.displayPastWaypoints = !config_.displayPastWaypoints; }
    
    void toggleFutureWaypoints()
        { config_.displayFutureWaypoints = !config_.displayFutureWaypoints; }

    void toggleDisplayPathWhenFinished()
        { config_.displayPathWhenFinished = !config_.displayPathWhenFinished; }

    void toggleDisplayPathWhenNotActivate()
        { config_.displayPathWhenNotActivated = !config_.displayPathWhenNotActivated; }
    
    void toggleDisplayWaypoints()
        {
            if (config_.displayWaypoints)
                config_.turnAllWpOff();
            else
                config_.turnAllWpOn();
        }

    void toggleOlympicMarker()
        { config_.displayOlympicMarker = !config_.displayOlympicMarker; }

    void setDisplayPastWaypoints( bool display )   
        { config_.displayPastWaypoints = display; }

    bool displayPastWaypoints()
        { return config_.displayPastWaypoints; }
    
    void setDisplayFutureWaypoints( bool display ) 
        { config_.displayFutureWaypoints = display; }

    bool displayFutureWaypoints()
        { return config_.displayFutureWaypoints; }

    void setDisplayPathWhenFinished( bool display )
        { config_.displayPathWhenFinished = display; }
        
    void setDisplayPathWhenNotActivated( bool display )
        { config_.displayPathWhenNotActivated = display; }

    bool displayPathWhenFinished()
        { return config_.displayPathWhenFinished; }

    bool displayPathWhenNotActivated()
        { return config_.displayPathWhenNotActivated; }
    
    void setDisplayWaypoints( bool display )       
        {
            if (display)
                config_.turnAllWpOn();
            else
                config_.turnAllWpOff();
        }

    bool displayWaypoints()
        { return config_.displayWaypoints; }

    bool displayOlympicMarker()
        { return config_.displayOlympicMarker; }

    void setUseTransparency( bool useTransparency )
        { config_.useTransparency = useTransparency; };

    bool useTransparency()
        { return config_.useTransparency; };
    
    void setColor( QColor color ) 
        { color_ = color; };
    
    void setFocus( bool inFocus ) 
        { inFocus_  = inFocus; };
    
    const hydroqguipath::GuiPath &currentPath() const 
        { return guiPath_; }; 
    
  private:

    void setRelativeStartTime( double relativeStartTime );
    
    hydroqguipath::GuiPath guiPath_;

    // The index of the waypoint we're currently pursuing
    int wpIndex_;
    orca::PathActivationEnum activationState_;

    PathFollowerPainterConfig config_;
    
    QColor color_;
    bool inFocus_;
    
    double relativeStartTime_;
    gbxiceutilacfr::Timer olympicMarkerTimer_;

    void drawOlympicMarker( QPainter *painter, float x, float y, double velocity );
    // returns true if marker is already at the goal, otherwise false
    bool olympicMarkerPosAndSpeed( float &x, float &y, double &velocity );

};

}

#endif
