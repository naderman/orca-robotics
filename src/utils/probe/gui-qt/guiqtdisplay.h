/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2006-2007 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef GUI_QT_DISPLAY_DRIVER_H
#define GUI_QT_DISPLAY_DRIVER_H

#include <vector>
#include <orcaqcm/ocmmodel.h>

#include <orcaprobe/idisplay.h>
#include <orcaprobe/ibrowser.h>

namespace probe
{

class MainWindow;

// need to be a QObject in order to post events into the Qt thread.
class GuiQtDisplay : public QObject, public orcaprobe::IDisplay
{
    Q_OBJECT
public:
    GuiQtDisplay( const std::vector<std::string> & supportedInterfaces );
    virtual ~GuiQtDisplay();

    // from orcaprobe::IDisplay
    virtual void enable( orcaprobe::IBrowser* browser );
    virtual void showNetworkActivity( bool isActive );
    virtual void setRegistryData( const orcacm::RegistryHierarchicalData1 & data );
    virtual void setPlatformData( const orcacm::RegistryHierarchicalData2 & data );
    virtual void setComponentData( const orcacm::ComponentData & data );
    virtual void setInterfaceData( const orcacm::InterfaceData & data );
    virtual void setOperationData( const orcacm::OperationData & data );
    virtual void setFocus( orcaprobe::IDisplay::FocusType focus );
    
private:
    std::vector<std::string>    supportedInterfaces_;
    orcaprobe::IBrowser    *browser_;
    orcaqcm::OcmModel           *model_;
    MainWindow                  *gui_;

};

} // namespace

#endif
