/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <QApplication>
#include <QDesktopWidget>

#include "mainwin.h"

int main(int argc, char** argv)
{
    QApplication app(argc,argv);

    // create GUI
    MainWindow m;
    m.resize(m.sizeHint());
    m.setCaption("G-Orca");
    // display GUI
    if ( QApplication::desktop()->width() > m.width() + 10
            && QApplication::desktop()->height() > m.height() +30 )
    {
        m.show();
    }
    else
    {
        m.showMaximized();
    }
    // setup quit condition
    QObject::connect( qApp, SIGNAL(lastWindowClosed()), qApp, SLOT(quit()) );
    // run
    return app.exec();
}

