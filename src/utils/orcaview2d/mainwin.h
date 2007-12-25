/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_ORCAQGUI_MAIN_WINDOW_H
#define ORCA_ORCAQGUI_MAIN_WINDOW_H

#include <vector>
#include <QMainWindow>

#include <hydroutil/jobqueue.h>
#include <orcaice/context.h>
#include <hydroqgui/ihumanmanager.h>
#include <orcaqguielementmodelview/guielementmodel.h>
#include <orcaqguielementmodelview/guielementview.h>

namespace orcaqcm {
    class OcmModel;
};

class QTreeView;
class QItemDelegate;
class QSplitter;
class QTimer;
class QComboBox;

// namespace orcaqgui 
// {

// configuration parameters for screen capture
typedef struct {
    int topPad;
    int sidePad;
    int bottomPad;
    std::string dumpPath;
    int captureTimerInterval;
} ScreenDumpParams;


//class GuiElementView;
//class ShortcutAction;

//
// Interaction with the JobQueue:
// - when you need to update the information about the content of the registry, create 
//   orcaqcm::GetComponentsJob and add it to the JobQueue. The job needs a pointer to an instance
//   of orcaqcm::OcmModel.
// - the rest will happen automatically in one of the threads of the JobQueue: the job will get
//   the data from the registry, transfer it to the OcmModel, which will update and repaint itself.
//
class MainWindow : public QMainWindow, public hydroqgui::IHumanManager
{
    Q_OBJECT
public:
    MainWindow( std::string                        title,   
                ScreenDumpParams                   screenDumpParams,
                int                                displayRefreshTime,
                const std::vector<std::string>    &supportedInterfaces,
                hydroutil::JobQueue               *jobQueue,
                const orcaice::Context            &context );

    // Returns the parent of the widget that displays everything graphically.
    QWidget *displayViewParent();

    void init( orcaqgemv::GuiElementModel *guiElemModel,
               QWidget                    *displayView );

    void loadElementsFromConfigFile( const orcaice::Context & context );

    // Inherited from hydroqgui::IHumanManager
    virtual void showBoxMsg( hydroqgui::IHumanManager::MessageType type, QString msg );
    virtual void showStatusMsg( hydroqgui::IHumanManager::MessageType type, QString msg );

    virtual QMenu    *fileMenu() { return fileMenu_; }
    virtual QMenu    *optionsMenu() { return optionsMenu_; }
    virtual QMenu    *displayMenu() { return displayMenu_; }
    virtual QToolBar *toolBar() { return toolBar_; }

//     virtual hydroqgui::IGuiElement *mouseEventReceiver() const { return mouseEventReceiver_; }
//     virtual bool requestBecomeMouseEventReceiver( hydroqgui::IGuiElement *requester );
//     virtual bool mouseEventReceiverIsSet();
//     virtual void relinquishMouseEventReceiver( hydroqgui::IGuiElement *relinquisher );
//    virtual GuiElementModel &guiElementModel() { return *elemModel_; }

    //    void changePlatformFocusFromView(const QString& platform);
    
//     virtual void subscribeToShortcutKey( QAction *elementAction, QKeySequence key, bool isMultiple, QObject *parent  );
//     virtual void unsubscribeFromShortcutKey( QKeySequence key, QObject *parent );

private slots:

    void updateRegistryView();
    void reloadRegistryView();
    
    void updateDisplayView();

    void changePlatformFocus(const QString&);
    // void changePlatformColor(const QString&);
    void addPlatformToList(const QString&);
    void removePlatformFromList(const QString&);
    
    // screen capture
    void toggleScreenCapture( bool capture );
    void grabWindow();

    void quit();
    void aboutOrca();
    void aboutApp();

private:

    QSplitter *win_;
    QSplitter *side_;

    // registry
    orcaqcm::OcmModel           *regModel_;
    QTreeView                   *regView_;
    QItemDelegate               *regDelegate_;
    
    // configuration parameters
    ScreenDumpParams screenDumpParams_;
    int displayRefreshTime_;

    // gui element list
    orcaqgemv::GuiElementModel             *elemModel_;
    orcaqgemv::GuiElementView              *elemView_;
    
    // display
    QWidget                     *displayView_;

    QTimer* regTimer_;
    QTimer* displayTimer_;
    std::vector<std::string>    supportedInterfaces_;
    
    // human input
    QAction* hiSelect_;
    QAction* hiWaypoints_;
    
    // platform focus box
    QToolBar* platformFocusToolbar_;
    QComboBox* platformCombo_;
    QComboBox* colorCombo_;
    
    bool firstTime_;
    
    void setupInterface();

    // screen capture
    void initScreenCapture();
    int screenDumpCounter_;
    QTimer* screenCaptureTimer_;
    
    QMenu* fileMenu_;
    QMenu* optionsMenu_;
    QMenu* displayMenu_;
    QToolBar *toolBar_;

//     // NULL means no-one is receiving mouse events
//     hydroqgui::IGuiElement *mouseEventReceiver_;
    
//    // list of shorcut actions
//    QList<ShortcutAction*> shortcutActions_;

    hydroutil::JobQueue*    jobQueue_;
    orcaice::Context        context_;
};


// }
#endif
