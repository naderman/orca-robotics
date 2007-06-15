/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2006-2007 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_PROBE_GUI_QT_DISPLAY_DRIVER_H
#define ORCA2_PROBE_GUI_QT_DISPLAY_DRIVER_H

#include <vector>
#include <orcaqcm/ocmmodel.h>

#include "../idisplay.h"
#include "../ibrowser.h"

namespace probe
{

class MainWindow;

// need to be a QObject in order to post events into the Qt thread.
class GuiQtDisplay : public QObject, public IDisplay
{
    Q_OBJECT
public:
    GuiQtDisplay( const std::vector<std::string> & supportedInterfaces );
    virtual ~GuiQtDisplay();

    // from IDisplay
    virtual void enable( IBrowser* browser );
    virtual void showNetworkActivity( bool isActive );
    virtual void setRegistryData( const orcacm::RegistryHierarchicalData1 & data );
    virtual void setPlatformData( const orcacm::RegistryHierarchicalData2 & data );
    virtual void setComponentData( const orcacm::ComponentData & data );
    virtual void setInterfaceData( const orcacm::InterfaceData & data );
    virtual void setOperationData( const orcacm::OperationData & data );
    virtual void setFocus( IDisplay::FocusType focus );
    
private:
    std::vector<std::string>    supportedInterfaces_;
    IBrowser    *browser_;
    orcaqcm::OcmModel           *model_;
    MainWindow                  *gui_;

};

} // namespace

#endif
