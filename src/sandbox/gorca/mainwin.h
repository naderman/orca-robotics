/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef GORCA_MAINWIN_H
#define GORCA_MAINWIN_H

#include <q3mainwindow.h>
#include <qdom.h>

#include <QComboBox>

#include <orcadef/componentdefinition.h>
#include <qorcauml/qorcamodel.h>

class QorcaCanvas;
class QorcaLibView;
class QorcaProjView;
class QorcaComponent;
class QorcaInterface;
class QLineEdit;

class MainWindow : public Q3MainWindow
{
    Q_OBJECT

public:
    MainWindow( QWidget* parent=0, const char* name=0, Qt::WFlags f=0 );
    ~MainWindow();

public slots:

private slots:
    bool initLibrary();
    void showAllLibrary();
    void filterLibrary();
    void printLibrary();

    void newProject();
    void openProject();
    void closeProject();
    void saveProject();
    void exportProject();
    void showProjectInfo();

    void selectAll();
    // a signal coming from the library sends the drop point
    // the no-input version is used by the menu command
    void copyLibToProj() { copyLibToProj( QPoint(200,150) ); };
    void copyLibToProj( const QPoint& );
    void validateSelected();

    void reloadView();
    void toggleLibrary();
    void enlarge();
    void shrink();
    void zoomIn();
    void zoomOut();
    void zoomReset();

    void aboutOrca();
    void aboutApp();

private:
    QList<orcadef::ComponentDef> library_;

    // The database representing the project
    QorcaModel projModel_;

    // The model of the library
    QorcaModel libraryModel_;

    // library of components: XMl, Canvas, Canvas View
    QDomDocument* libDom_;
    QorcaCanvas* libCanvas_;
    QorcaLibView* libView_;

    // current project: XMl, Canvas, Canvas View
    QDomDocument* projDom_;
    QorcaCanvas* projCanvas_;
    QorcaProjView* projView_;
    QString projFileName_;

    // interface elements
    QLineEdit *nameFilter_;
    QComboBox *interfaceFilter_;
    QComboBox *providedRequiredFilter_;

    void setupProjectDir();
    int exportConfigFiles();
    int exportSystemFile();
    int exportHostFiles();
    int exportScriptFile();

    QorcaInterface* lookupPort( const int pid );

    void initInterface();

    void quit();
};

#endif
