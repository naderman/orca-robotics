/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#ifndef ORCA_ORCAQT_UTILS_H
#define ORCA_ORCAQT_UTILS_H

namespace orcaqt
{

/*!
    Similar to QMessageBox::aboutQt(). Displays an "about" message commonly 
    linked from the Help menu.
*/
void aboutOrca( QWidget* parent, const QString & title = QString() );


/*!
    A basic "About Application" dialog which uses Orca icon. Adds the URL of
    of the Orca website to the @p text.
*/
void aboutApp( QWidget* parent, const QString & title, const QString & text );

} // namespace

#endif
