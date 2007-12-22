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
#include <orcaqgui/ihumanmanager.h>

namespace orcaqcm {
    class OcmModel;
};

class QTreeView;
class QItemDelegate;
class QSplitter;
class QTimer;
class QComboBox;

namespace orcaqgui 
{

// configuration parameters for screen capture
typedef struct {
    int topPad;
    int sidePad;
    int bottomPad;
    std::string dumpPath;
    int captureTimerInterval;
} ScreenDumpParams;


class GuiElementView;
class ShortcutAction;

class MainWindow : public QMainWindow, public orcaqgui::IHumanManager
{
    Q_OBJECT
public:
    MainWindow( std::string                        title,   
                ScreenDumpParams                   screenDumpParams,
                int                                displayRefreshTime,
                const std::vector<std::string>    &supportedInterfaces,
                hydroutil::JobQueue               *jobQueue,
                const orcaice::Context            &context );
//                 QWidget                           *parent = 0, 
//                 Qt::WFlags                         flags = 0 );

    // Returns the parent of the widget that displays everything graphically.
    QWidget *displayViewParent();

    void init( orcaqgui::GuiElementModel           *guiElemModel,
               QWidget                            *displayView );

    void loadElementsFromConfigFile( const orcaice::Context & context );

    // Inherited from IHumanManager
    virtual void showBoxMsg( orcaqgui::MessageType type, QString msg );
    virtual void showStatusMsg( orcaqgui::MessageType type, QString msg );

    virtual QMenu    *fileMenu() { return fileMenu_; }
    virtual QMenu    *optionsMenu() { return optionsMenu_; }
    virtual QMenu    *displayMenu() { return displayMenu_; }
    virtual QToolBar *toolBar() { return toolBar_; }
    
    virtual GuiElement *modeOwner() const { return modeOwner_; }
    virtual bool requestMode( GuiElement *requester );
    virtual bool modeIsOwned();
    virtual void relinquishMode( GuiElement *relinquisher );

    virtual GuiElementModel &guiElementModel() { return *elemModel_; }

    void changePlatformFocusFromView(const QString& platform);
    
    virtual void subscribeToKey( QAction *elementAction, QKeySequence key, bool isMultiple, QObject *parent  );
    virtual void unsubscribeFromKey( QKeySequence key, QObject *parent );

private slots:

    void updateRegistryView();
    void reloadRegistryView();
    
    void updateDisplayView();

    void changePlatformFocus(const QString&);
    void changePlatformColor(const QString&);
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
//     orcaqcm::NetworkThread     *networkThread_;
//     orcaqcm::ModelThread       *regModelThread_;
    orcaqcm::OcmModel           *regModel_;
    QTreeView                   *regView_;
    QItemDelegate               *regDelegate_;
    
    // configuration parameters
    ScreenDumpParams screenDumpParams_;
    int displayRefreshTime_;

    // gui element list
    GuiElementModel             *elemModel_;
    GuiElementView              *elemView_;
    
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

    // NULL means mode is not owned
    GuiElement *modeOwner_;
    
    // list of shorcut actions
    QList<ShortcutAction*> shortcutActions_;

    hydroutil::JobQueue*    jobQueue_;
    orcaice::Context        context_;
};


}
#endif
