/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
 
#ifndef ORCA2_ORCAQT_UTILS_H
#define ORCA2_ORCAQT_UTILS_H

namespace orcaqt
{

/*!
    Similar to QMessageBox::aboutQt(). Displays an "about" message commonly 
    linked from the Help menu.
*/
void aboutOrca( QWidget* parent, const QString & title = QString() );

} // namespace

#endif
