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

#ifndef GORCA_REPOSITORY_DOM_H
#define GORCA_REPOSITORY_DOM_H

#include <qdom.h>

class QorcaRepositoryDom : public QDomDocument
{
public:
    QorcaRepositoryDom( const QString fileName );
    ~QorcaRepositoryDom();

private:
    void parseHeader( const QDomElement &header );
    void parseRepository( const QDomElement &parentElement );
    void parseComponent( const QDomElement &parentElement );
    void parsePort( const QDomElement &parentElement );
};

#endif // QORCA_REPOSITORY_DOM_H
