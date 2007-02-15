/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ORCAQGUI_GUI_ELEMENT_H
#define ORCA2_ORCAQGUI_GUI_ELEMENT_H

#include <string>
#include <assert.h>
#include <iostream>

#include <QString>
#include <QColor>
#include <QPointF>
#include <QMouseEvent>
#include <QMatrix>

#include <orcanavutil/orcanavutil.h>
#include <orcaobj/mathdefs.h>

namespace orcaqgui
{
       
/*!
 *
 * @brief Anyone who wants to get painted should derive from this.
 *
 * GuiElement is a basic unit which knows how to paint itself and stores its data.
 * The GuielementModel holds a list of these GuiElements and periodically paints and updates them.
*/
class GuiElement
{                                 
public:
    
    GuiElement() : platform_(""),
                   details_(""),
                   name_("")
                   {};
    
    virtual ~GuiElement() {};

    //! Determines whether the element should be painted in the global coordinate system.
    //! If returns FALSE, we will search for a localization element on the same platform.
    virtual bool isInGlobalCS()=0;

    //! Update is called periodically. All processing different from paint can be done here
    virtual void update () {};

    //! Optional function to populate the menu list on right-click. 
    //! Default implementation does nothing.
    virtual QStringList contextMenu() { return QStringList(); };
    
    //! Optional callback function which will be called when one of the context menu items
    //! is selected. Default implementation does nothing.
    virtual void execute( int action ) {};
    
    //! Set the colour of the element.
    //! This sets the general colour scheme, not necessarily the colour of the entire
    //! element. In fact, the guiElement is free to ignore this colour.
    virtual void setColor( QColor color ) {};

    //! Optional function to set focus of element. Default implementation does nothing.
    //! Focus determines how the element is painted (color or gray) 
    //! and whether users can interact with it (buttons or no buttons)
    virtual void setFocus( bool inFocus ) {};
    
    //! Called when the GuiElement ceases to be the owner of the global user-input mode 
    //! NOTE: This is _not_ called if the GuiElement voluntarily relinquishes the mode.
    virtual void lostMode() {assert(false&&"unimplemented lostMode");}

    virtual void mousePressEvent(QMouseEvent *e) {assert(false&&"unimplemented mouse function");}
    virtual void mouseMoveEvent(QMouseEvent *e) {assert(false&&"unimplemented mouse function");}
    virtual void mouseReleaseEvent(QMouseEvent *e) {assert(false&&"unimplemented mouse function");}
    virtual void mouseDoubleClickEvent(QMouseEvent *e) {assert(false&&"unimplemented mouse function");} 
    
    //! Tell the GuiElement whether to paint itself using transparent colors
    virtual void setTransparency( bool useTransparency ) {};
    
    //! Access and set functions for platform name
    QString platform() { return platform_; };
    void setPlatform( QString platform ) { platform_ = platform; };
    
    //! Access and set functions for details string displayed in table, e.g. ::orca::OgMap
    QString details() { return details_; };
    void setDetails( QString details ) { details_ = details; };
    
    //! Access and set functions for name string displayed in table, e.g. ogmap@hemp/ogmaploader
    QString name() { return name_; };
    void setName( QString name ) { name_ = name; };
    
    //! Sets the 2d offset of the Gui coordinate system wrt the (aribitrarily defined) global reference frame
    void setGuiCsOffset( double x, double y, double theta ) { xGuiOff_=x; yGuiOff_=y; thetaGuiOff_=theta; };
    
private:
    QString platform_;
    QString details_;
    QString name_;
    
protected:
    double xGuiOff_;
    double yGuiOff_;
    double thetaGuiOff_;
    
    //! Transforms a pose from an element-specific, local CS 
    //! (received from an interface) into the GUI CS.
    //! Does 2 transformations: local-to-global CS, then global-to-GUI CS
    inline void transformToGuiCs( double  xIn, 
                                 double  yIn, 
                                 double  thetaIn,
                                 double  xOff, 
                                 double  yOff, 
                                 double  thetaOff, 
                                 double &xOut, 
                                 double &yOut,
                                 double &thetaOut )
    {
        double xGlobal, yGlobal;
        
        orcanavutil::transformPoint2d( xIn, yIn, xOff, yOff, thetaOff, xGlobal, yGlobal);
        orcanavutil::transformPoint2d( xGlobal, yGlobal, xGuiOff_, yGuiOff_, thetaGuiOff_, xOut, yOut );
        
        thetaOut = thetaIn + thetaOff + thetaGuiOff_;
        orcanavutil::normaliseAngle(thetaOut);
        
        std::cout << "xIn, yIn:                      " << xIn << " " << yIn << std::endl;
        std::cout << "xOff, yOff, thetaOff:          " << xOff << " " << yOff << " " << thetaOff << std::endl;
        std::cout << "xGlobal, yGlobal:              " << xGlobal << " " << yGlobal << std::endl;
        std::cout << "xGuiOff_, yGuiOff_,thetaGuiOff_" << xGuiOff_ << " " << yGuiOff_ << " " << thetaGuiOff_ << std::endl;
        std::cout << "xOut, yOut, thetaOut:          " << xOut << " " << yOut << thetaOut << std::endl;
    };
    
};

} // namespace

#endif
