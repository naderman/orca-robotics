/*
 *  Orca Project: Components for robotics.
 *
 *  Copyright (C) 2004-2006
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
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
